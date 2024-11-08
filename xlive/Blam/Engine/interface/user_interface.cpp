#include "stdafx.h"
#include "user_interface.h"
#include "game/game.h"
#include "cutscene/cinematics.h"

const bool __cdecl user_interface_automation_is_active()
{
	return false;
}

uint32 __cdecl user_interface_milliseconds()
{
	return INVOKE(0x2096AE, 0x0, user_interface_milliseconds);
}
bool __cdecl user_interface_error_display_allowed()
{
	//return INVOKE(0x21D441, 0x0, user_interface_error_display_allowed);

	bool result = !cinematic_in_progress();
	if (game_in_progress() && game_is_ui_shell())
		result = true;
	return result;
}

bool __cdecl user_interface_has_responding_controller(int32 user_index)
{
	return INVOKE(0x20A20C, 0x0, user_interface_has_responding_controller, user_index);
}

bool __cdecl user_interface_channel_is_busy(e_user_interface_channel_type channel_type)
{
	return INVOKE(0x20A540, 0x0, user_interface_channel_is_busy, channel_type);
}

bool __cdecl user_interface_in_screen(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, e_user_interface_screen_id screen_id)
{
	return INVOKE(0x20BAAA, 0x0, user_interface_in_screen, channel_type, window_index, screen_id);
}

bool __cdecl user_interface_error_screen_is_active(e_user_interface_channel_type channel_index, e_user_interface_render_window window_index, e_ui_error_types error_code)
{
	return INVOKE(0x20A77D, 0x0, user_interface_error_screen_is_active, channel_index, window_index, error_code);
}

void __cdecl screen_error_ok_dialog_show(e_user_interface_channel_type channel_type, e_ui_error_types ui_error_index, e_user_interface_render_window window_index, uint16 user_flags, void* ok_callback, void* fallback)
{
	INVOKE(0x20E1D8, 0x0, screen_error_ok_dialog_show, channel_type, ui_error_index, window_index, user_flags, ok_callback, fallback);
	return;
}

void __cdecl screen_error_ok_dialog_with_custom_text(e_user_interface_channel_type channel_type, e_ui_error_types ui_error_index, e_user_interface_render_window window_index, uint16 user_flags, void* ok_callback, void* fallback, const wchar_t* custom_title, wchar_t* const custom_body)
{
	return INVOKE(0x20E1DA, 0x0, screen_error_ok_dialog_with_custom_text, channel_type, ui_error_index, window_index, user_flags, ok_callback, fallback, custom_title, custom_body);
}

void __cdecl user_interface_error_display_ok_cancel_dialog_with_ok_callback(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, uint16 user_flags, void* ok_callback_handle, e_ui_error_types error_type)
{
	INVOKE(0x20E3BB, 0x0, user_interface_error_display_ok_cancel_dialog_with_ok_callback, channel_type, window_index, user_flags, ok_callback_handle, error_type);
}

void __cdecl user_interface_back_out_from_channel(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index)
{
	INVOKE(0x2096DA, 0x0, user_interface_back_out_from_channel, channel_type, window_index);
}

bool __cdecl user_interface_back_out_from_channel_by_id(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, e_user_interface_screen_id id)
{
	return INVOKE(0x209850, 0x0, user_interface_back_out_from_channel_by_id, channel_type, window_index, id);
}

void __cdecl user_interface_enter_game_shell(int32 context)
{
	INVOKE(0x20CE70, 0x0, user_interface_enter_game_shell, context);
}



void render_menu_user_interface_to_usercall(int32 window_index, int32 controller_index, int32 player_count, rectangle2d* rect2d)
{
	static void* render_menu_user_interface = (void*)Memory::GetAddress(0x20B697);
	__asm {
		push rect2d
		push player_count
		push controller_index
		mov esi, window_index
		call render_menu_user_interface
		add esp, 12
	}
	return;
}
