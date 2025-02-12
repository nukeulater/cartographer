#include "stdafx.h"
#include "user_interface_controller.h"

#include "user_interface.h"
#include "user_interface_globals.h"
#include "user_interface_utilities.h"
#include "user_interface_widget_window.h"
#include "user_interface_guide.h"

#include "game/game_time.h"
#include "networking/online/online_account_xbox.h"
#include "saved_games/cartographer_player_profile.h"
#include "scenario/scenario_definitions.h"
#include "scenario/scenario.h"
#include "sound/sound_manager.h"
#include "tag_files/global_string_ids.h"
#include "tag_files/string_id.h"
#include "text/unicode.h"

/* typedefs */

typedef bool(__cdecl* t_user_interface_controller_sign_in)(e_controller_index controller_index, s_saved_game_player_profile* profile, uint32 enumerated_file_index);
typedef void(__cdecl* t_user_interface_controller_sign_out)(e_controller_index controller_index);

/* globals */

t_user_interface_controller_sign_in p_user_interface_controller_sign_in;
t_user_interface_controller_sign_out p_user_interface_controller_sign_out;

/* private prototypes */

static bool __cdecl user_interface_controller_verify_reconnection(void);
static bool __cdecl user_interface_controller_verify_reconnection_failed(c_screen_widget* error_screen);
static void user_interface_controller_process_events(uint32 elapsed_time);
static bool user_inteface_controller_has_removed_screen_active(void);
static void user_interface_controller_update_disconnect(void);
static void user_interface_controller_removed_handler(void);
static void user_interface_controller_boot_to_dash_check(void);

/* public code */

void user_interface_controller_apply_patches(void)
{
	PatchCall(Memory::GetAddress(0x20887A), user_interface_controller_update_player_name); // fixes guest-signin names in ONLINE mode
	NopFill(Memory::GetAddress(0x20CF20), 6); // fixes auto guest-signout when leaving a match
	WriteValue<uint8>(Memory::GetAddress(0x20CEB5 + 6), 0); // disable _ui_error_demo_version_no_more_for_you
	DETOUR_ATTACH(p_user_interface_controller_sign_in, Memory::GetAddress<t_user_interface_controller_sign_in>(0x2087BF), user_interface_controller_sign_in);
	DETOUR_ATTACH(p_user_interface_controller_sign_out, Memory::GetAddress<t_user_interface_controller_sign_out>(0x208257, 0x1F491B), user_interface_controller_sign_out);
	PatchCall(Memory::GetAddress(0x20CB4B), user_interface_controller_update); // fix infinite controller-disconnect looping
	return;
}

s_user_interface_controller_globals* user_interface_controller_globals_get(void)
{
	return Memory::GetAddress<s_user_interface_controller_globals*>(0x96C858, 0x999038);
}

void __cdecl user_interface_controller_initialize(void)
{
	INVOKE(0x208608, 0x0, user_interface_controller_initialize);
	return;
}

void __cdecl user_interface_controller_update(void)
{
	//INVOKE(0x208C5D, 0x0, user_interface_controller_update);

	s_user_interface_controller_globals* g_user_interface_controller_globals = user_interface_controller_globals_get();
	uint32 time_elapsed = user_interface_milliseconds();

	if (g_user_interface_controller_globals->event_records[_controller_index_0].type)
	{
		user_interface_controller_event_submit(&g_user_interface_controller_globals->event_records[_controller_index_0]);

		//progressively move 3->2->1->0 and submit data on 0
		int8 count = 0;
		do
		{
			csmemcpy(&g_user_interface_controller_globals->event_records[_controller_index_0 + count],
				&g_user_interface_controller_globals->event_records[_controller_index_0 + count + 1],
				sizeof(s_event_record));
			count++;
		} while (count < _controller_index_3);

		csmemset(&g_user_interface_controller_globals->event_records[_controller_index_3], 0, sizeof(s_event_record));
	}

	else if (!g_user_interface_controller_globals->event_manager_suppress)
	{
		user_interface_controller_process_events(time_elapsed);
	}

	user_interface_controller_update_disconnect();

	//
	// user_interface_controller_update_online_tasks();
	// TODO: when we start working on online_tasks
	//

	if (!input_windows_processing_device_change())
	{
		// dont run this when we are adjusting device ports
		user_interface_controller_removed_handler();
	}
	user_interface_controller_boot_to_dash_check();
	return;
}

/* public code */

bool __cdecl user_interface_controller_is_player_profile_valid(e_controller_index controller_index)
{
	//return INVOKE(0x206B50, 0x1F3F78, user_interface_controller_is_player_profile_valid, controller_index);
	return user_interface_controller_globals_get()->controllers[controller_index].m_flags.test(_controller_state_has_valid_profile_bit);
}

e_controller_index __cdecl user_interface_controller_get_next_valid_index(e_controller_index controller_index)
{
	return INVOKE(0x206B13, 0x1F3F3A, user_interface_controller_get_next_valid_index, controller_index);
}

uint32 __cdecl user_interface_controller_get_user_index(e_controller_index controller_index)
{
	return INVOKE(0x20687F, 0x1F3CE8, user_interface_controller_get_user_index, controller_index);
}

void __cdecl user_interface_controller_set_user_index(e_controller_index controller_index, uint32 user_index)
{
	INVOKE(0x207342, 0x1F43F2, user_interface_controller_set_user_index, controller_index, user_index);
	return;
}

e_controller_index __cdecl user_interface_controller_get_controller_for_user(uint32 user_index)
{
	return INVOKE(0x207365, 0x1F4415, user_interface_controller_get_controller_for_user, user_index);
}

e_controller_index __cdecl user_interface_controller_get_first_valid_controller(void)
{
	e_controller_index main_controller = k_no_controller;

	for (e_controller_index controller = first_controller();
		controller != k_no_controller;
		controller = next_controller(controller))
	{
		if (user_interface_controller_is_player_profile_valid(controller))
		{
			main_controller = controller;
			break;
		}

	}
	return main_controller;
}

e_game_team __cdecl user_interface_controller_get_user_active_team(e_controller_index controller_index)
{
	return INVOKE(0x206907, 0, user_interface_controller_get_user_active_team, controller_index);
}

void __cdecl user_interface_controller_set_desired_team_index(e_controller_index controller_index, e_game_team team)
{
	INVOKE(0x2068F2, 0x0, user_interface_controller_set_desired_team_index, controller_index, team);
	return;
}

bool __cdecl user_interface_controller_get_rumble_enabled(e_controller_index controller_index)
{
	return INVOKE(0x207600, 0x0, user_interface_controller_get_rumble_enabled, controller_index);
}

bool __cdecl user_interface_controller_get_autolevel_enabled(e_controller_index controller_index)
{
	return INVOKE(0x207627, 0, user_interface_controller_get_autolevel_enabled, controller_index);
}

e_handicap __cdecl user_interface_controller_get_user_handicap_level(e_controller_index controller_index)
{
	return INVOKE(0x206938, 0, user_interface_controller_get_user_handicap_level, controller_index);
}

void __cdecl user_interface_controller_set_user_handicap_level(e_controller_index controller_index, e_handicap handicap)
{
	INVOKE(0x206923, 0, user_interface_controller_set_user_handicap_level, controller_index, handicap);
	return;
}

void __cdecl user_interface_controller_set_griefer(e_controller_index controller_index, bool griefing)
{
	INVOKE(0x206949, 0, user_interface_controller_set_griefer, controller_index, griefing);
	return;
}

wchar_t* __cdecl user_interface_controller_get_player_profile_name(e_controller_index controller_index)
{
	return INVOKE(0x206B67, 0, user_interface_controller_get_player_profile_name, controller_index);
}

uint32 __cdecl user_interface_controller_get_signed_in_controller_count(void)
{
	return INVOKE(0x2073AE, 0, user_interface_controller_get_signed_in_controller_count);
}

uint16 __cdecl user_interface_controller_get_signed_in_controllers_mask(void)
{
	return INVOKE(0x20758D, 0, user_interface_controller_get_signed_in_controllers_mask);
}

uint32 __cdecl user_interface_controller_get_last_level_played(e_controller_index controller_index)
{
	return INVOKE(0xFE106, 0, user_interface_controller_get_last_level_played, controller_index);
}

uint32 __cdecl user_interface_controller_get_highest_campaign_level_in_signed_in_controllers()
{
	return INVOKE(0x2076F7, 0, user_interface_controller_get_highest_campaign_level_in_signed_in_controllers);
}

bool __cdecl user_interface_controller_sign_in(e_controller_index controller_index, s_saved_game_player_profile* profile, uint32 enumerated_file_index)
{
	bool result = p_user_interface_controller_sign_in(controller_index, profile, enumerated_file_index);
	if (result)
		cartographer_player_profile_sign_in(controller_index, enumerated_file_index);

	return result;
}

void __cdecl user_interface_controller_sign_out(e_controller_index controller_index)
{
	p_user_interface_controller_sign_out(controller_index);
	cartographer_player_profile_sign_out(controller_index);
	return;
}

void __cdecl user_interface_controller_sign_out_all_controllers()
{
	INVOKE(0x208A28, 0x1F4E9F, user_interface_controller_sign_out_all_controllers);
	return;
}

void __cdecl user_interface_controller_get_profile_data(e_controller_index controller_index, s_saved_game_player_profile* profile, uint32* profile_index)
{
	INVOKE(0x206890, 0x0, user_interface_controller_get_profile_data, controller_index, profile, profile_index);
	return;
}

void __cdecl user_interface_controller_get_user_properties(e_controller_index controller_index, s_player_identifier* controller_user_identifier, s_player_properties* properties)
{
	INVOKE(0x20696F, 0x0, user_interface_controller_get_user_properties, controller_index, controller_user_identifier, properties);
	return;
}

void __cdecl user_interface_controller_event_submit(s_event_record* event)
{
	INVOKE(0x20D0C5, 0x0, user_interface_controller_event_submit, event);
	return;
}

void __cdecl user_interface_controller_update_network_properties(e_controller_index controller_index)
{
	INVOKE(0x206A97, 0x0, user_interface_controller_update_network_properties, controller_index);
	return;
}

void __cdecl user_interface_controller_pick_profile_dialog(e_controller_index controller_index, bool online_user)
{
	// second argument is not used by the function although it is pushed to the stack
	// TODO : reimplement this to make use of the second argument
	INVOKE(0x209236, 0x0, user_interface_controller_pick_profile_dialog, controller_index, online_user);
	return;
}

bool __cdecl user_interface_controller_pick_profile_offline(e_controller_index controller_index)
{
	// INVOKE(0x212F56, 0x0, user_interface_controller_pick_profile_dialog_wrapper, controller_index);
	user_interface_controller_pick_profile_dialog(controller_index, false);
	return true;
}

bool __cdecl user_interface_controller_has_gamepad(e_controller_index controller_index)
{
	if (VALID_INDEX(controller_index, k_number_of_controllers))
	{
		return input_has_gamepad(controller_index, nullptr);
		//return input_has_gamepad_plugged(controller_index);
	}
	return false;
}

bool __cdecl user_interface_controller_has_gamepad_just_left(e_controller_index controller_index)
{
	if (VALID_INDEX(controller_index, k_number_of_controllers))
	{
		return input_gamepad_just_left(controller_index);
	}
	return false;
}

bool __cdecl user_interface_controller_is_guest(e_controller_index controller_index)
{
	s_user_interface_controller_globals* g_user_interface_controller_globals = user_interface_controller_globals_get();
	XUID* identifier = (XUID*)&g_user_interface_controller_globals->controllers[controller_index].controller_user_identifier;
	if (!ONLINE_USER_VALID(*identifier))
		return false;

	return online_xuid_is_guest_account(*identifier);
}

uint32 __cdecl user_interface_controller_get_guest_controllers_count_for_master(e_controller_index master_controller_index)
{
	if (user_interface_controller_is_guest(master_controller_index))
		return 0;


	s_user_interface_controller_globals* g_user_interface_controller_globals = user_interface_controller_globals_get();
	XUID master_identifier = *(XUID*)&g_user_interface_controller_globals->controllers[master_controller_index].controller_user_identifier;
	if (!ONLINE_USER_VALID(master_identifier))
		return 0;

	uint32 count = 0;
	for (e_controller_index controller_idx = first_controller();
		controller_idx != k_no_controller;
		controller_idx = next_controller(controller_idx))
	{
		if (controller_idx == master_controller_index)
			continue;


		if (user_interface_controller_has_xbox_live(controller_idx))
		{
			s_player_identifier player_id = g_user_interface_controller_globals->controllers[controller_idx].controller_user_identifier;
			XUID compare_id = *(XUID*)&g_user_interface_controller_globals->controllers[controller_idx].controller_user_identifier;
			if (!ONLINE_USER_VALID(compare_id))
				continue;

			if ((compare_id & ~0x3ULL) == (master_identifier & ~0x3ULL))
				count++;
		}

	}

	return count;

}

bool __cdecl user_interface_controller_has_xbox_live(e_controller_index controller_index)
{
	return user_interface_controller_globals_get()->controllers[controller_index].m_flags.test(_controller_state_has_xbox_live_bit);
}

void __cdecl user_interface_controller_xbox_live_account_set_signed_in(e_controller_index controller_index, bool active)
{
	//INVOKE(0x208A01, 0x0, user_interface_controller_xbox_live_account_set_signed_in, controller_index, active);
	s_user_interface_controller* controller = &user_interface_controller_globals_get()->controllers[controller_index];
	if (active)
	{
		controller->m_flags.set(_controller_state_has_xbox_live_bit, true);
	}
	else
	{
		controller->m_flags.set(_controller_state_has_xbox_live_bit, false);

		// not calling update_name here to prevent recursion lock
		//user_interface_controller_update_player_name(controller_index);
	}
	return;
}


void __cdecl user_interface_controller_update_player_name(e_controller_index controller_index)
{
	// INVOKE(0x208312, 0x0, user_interface_controller_update_player_name, controller_index);

	s_user_interface_controller* controller = &user_interface_controller_globals_get()->controllers[controller_index];
	c_user_interface_guide_state_manager* guide = user_interface_guide_state_manager_get();
	if (online_connected_to_xbox_live())
	{
		XUID* controller_xuid = (XUID*)(&controller->controller_user_identifier);
		if (online_xuid_is_guest_account(*controller_xuid))
		{
			uint8 guest_no = online_xuid_get_guest_account_number(*controller_xuid);
			wchar_t format[512];
			user_interface_global_string_get(_string_id_guest_of_ascii_gamertag_unicode_format_string, format);// %d %hs
			usnzprintf(controller->player_name,
				NUMBEROF(controller->player_name),
				format,
				guest_no,
				guide->m_gamertag);

		}
		else
		{
			usnzprintf(controller->player_name,
				NUMBEROF(controller->player_name),
				L"%hs",
				guide->m_gamertag);

		}

		//todo move this out of here and figure out why the guide live signin fails to set the bit
		user_interface_controller_xbox_live_account_set_signed_in(controller_index, true);
	}
	else if (user_interface_controller_is_player_profile_valid(controller_index))
	{
		ustrncpy(controller->player_name, controller->player_profile.name, NUMBEROF(controller->player_profile.name));
	}
	else
	{
		controller->player_name[0] = '\0';
	}
	user_interface_controller_update_network_properties(controller_index);
	return;
}

/* private code */

static bool __cdecl user_interface_controller_verify_reconnection(void)
{
	return INVOKE(0x20841E, 0x0, user_interface_controller_verify_reconnection);
}

static bool __cdecl user_interface_controller_verify_reconnection_failed(c_screen_widget* error_screen)
{
	return INVOKE(0x208485, 0x0, user_interface_controller_verify_reconnection_failed, error_screen);
}

static void __cdecl user_interface_controller_process_events(uint32 elapsed_time)
{
	INVOKE(0x207750, 0x0, user_interface_controller_process_events, elapsed_time);
	return;
}

static bool user_inteface_controller_has_removed_screen_active(void)
{
	//h2v only checked for _ui_error_controller_removed
	if (user_interface_error_screen_is_active(_user_interface_channel_type_hardware_error, _window_4, _ui_error_controller_removed)
		|| user_interface_error_screen_is_active(_user_interface_channel_type_hardware_error, _window_4, _ui_error_controller1_removed)
		|| user_interface_error_screen_is_active(_user_interface_channel_type_hardware_error, _window_4, _ui_error_controller2_removed)
		|| user_interface_error_screen_is_active(_user_interface_channel_type_hardware_error, _window_4, _ui_error_controller3_removed)
		|| user_interface_error_screen_is_active(_user_interface_channel_type_hardware_error, _window_4, _ui_error_controller4_removed))
	{

		return true;
	}
	return false;
}

static void user_interface_controller_update_disconnect(void)
{
	//redone to fix infinite controller disconnection error bug

	s_user_interface_controller_globals* g_user_interface_controller_globals = user_interface_controller_globals_get();

	for (e_controller_index controller = first_controller();
		controller != k_no_controller;
		controller = next_controller(controller))
	{

		s_user_interface_controller& controller_data = g_user_interface_controller_globals->controllers[controller];
		bool bad_condition = !input_has_gamepad_plugged(controller);

		if (controller == k_windows_device_controller_index)
		{
			bool is_simulated;
			bool device_connected = input_has_gamepad(controller, &is_simulated);

			bad_condition = is_simulated &&
				input_abstraction_get_last_used_device(controller) == k_last_used_device_was_gamepad || !device_connected;

			if (input_windows_has_split_device_active()
				|| user_interface_in_screen(_user_interface_channel_type_gameshell_screen, _window_4, _screen_4way_join_screen))
			{
				bad_condition = false; // prevent 4way screen from becoming a sphagetti
			}
		}

		if (bad_condition)
		{
			const bool result = (user_interface_controller_is_player_profile_valid(controller) || user_interface_has_responding_controller(controller))
				&& !user_interface_automation_is_active();

			g_user_interface_controller_globals->controller_detached[controller] = result;

			controller_data.m_flags.set(_controller_state_is_attached_bit, false);
			controller_data.m_flags.set(_controller_state_bit1, false);
			controller_data.m_flags.set(_controller_state_bit2, false);
		}
		else
		{
			controller_data.m_flags.set(_controller_state_is_attached_bit, true);
			g_user_interface_controller_globals->controller_detached[controller] = false;

			int8 out_drive_letter;
			const bool test_drive1 = input_windows_drive_letter_test(1 + controller * 2, &out_drive_letter);
			const bool test_drive2 = input_windows_drive_letter_test(2 + controller * 2, &out_drive_letter);
			//bool has_voice_active= voice_enabled_test(controller); //TODO

			controller_data.m_flags.set(_controller_state_bit1, test_drive1);
			controller_data.m_flags.set(_controller_state_bit2, test_drive2);
			//controller_data.m_flags.set(_controller_state_has_voice_bit, test_drive2); //TODO : also find this voice_bit on h2v

		}
	}
	return;
}

static void user_interface_controller_removed_handler(void)
{
	//INVOKE(0x208738, 0x0, user_interface_controller_removed_handler);
	// redone to display proper error msgs

	s_user_interface_controller_globals* g_user_interface_controller_globals = user_interface_controller_globals_get();

	int16 detached_controller = _controller_index_0;
	while (!g_user_interface_controller_globals->controller_detached[detached_controller])
	{
		if (++detached_controller >= k_number_of_controllers)
			return;
	}
	if (!user_inteface_controller_has_removed_screen_active() && user_interface_error_display_allowed())
	{
		//e_ui_error_types error_msg = _ui_error_keyboard_removed; // use when we add support for multiple kbm?
		e_ui_error_types error_msg = _ui_error_controller_removed;
		switch (detached_controller)
		{
		case _controller_index_0:
			detached_controller = _ui_error_controller1_removed;
			break;
		case _controller_index_1:
			detached_controller = _ui_error_controller2_removed;
			break;
		case _controller_index_2:
			detached_controller = _ui_error_controller3_removed;
			break;
		case _controller_index_3:
			detached_controller = _ui_error_controller4_removed;
			break;
		}

		screen_error_ok_dialog_show(
			_user_interface_channel_type_hardware_error,
			error_msg,
			_window_4,
			FLAG(detached_controller),
			user_interface_controller_verify_reconnection,
			user_interface_controller_verify_reconnection_failed);

		const scenario* scnr = global_scenario_get();

		if (scnr && scnr->type == scenario_type_singleplayer && !game_time_get_paused())
		{
			game_time_set_paused(true);
			sound_pause(false);
		}
	}
	return;
}

static void user_interface_controller_boot_to_dash_check(void)
{
	//basically ALT (SYSKEYDOWN) needs to be held earlier than F4
	if ((input_windows_key_frames_down(VK_F4) == 1
		&& input_windows_key_frames_down(WM_SYSKEYDOWN) != 0)
		|| *g_input_windows_request_terminate == true)
	{
		e_ui_error_types error_id = _ui_error_confirm_boot_to_dash;
		void* exit_callback = user_interface_exit_to_dash;

		if (user_interface_globals_get_map_type() == scenario_type_singleplayer)
		{
			error_id = _ui_error_confirm_save_and_exit_campaign;
			exit_callback = user_interface_save_map_and_exit;
		}

		user_interface_error_display_ok_cancel_dialog_with_ok_callback(
			//_user_interface_channel_type_dialog, //orignally this , but changed to hardware_error so it can overrule disconnection screen
			_user_interface_channel_type_hardware_error,
			_window_4,
			NONE,
			exit_callback,
			error_id);

		*g_input_windows_request_terminate = false;
	}
	return;
}

