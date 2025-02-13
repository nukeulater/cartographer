#include "stdafx.h"
#include "user_interface.h"

#include "game/game.h"
#include "cutscene/cinematics.h"

#include "XLive/xbox/xbox.h"

/* public code */

void user_interface_apply_patches(void)
{
	PatchCall(Memory::GetAddress(0x39C7A), user_interface_update);
	return;
}

bool __cdecl user_interface_automation_is_active(void)
{
	return false;
}

uint32 __cdecl user_interface_milliseconds(void)
{
	return INVOKE(0x2096AE, 0x0, user_interface_milliseconds);
}
bool __cdecl user_interface_error_display_allowed(void)
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

void __cdecl screen_error_ok_dialog_with_custom_text(e_user_interface_channel_type channel_type, e_ui_error_types ui_error_index, e_user_interface_render_window window_index, uint16 user_flags, void* ok_callback, void* fallback, const wchar_t* custom_title, const wchar_t* custom_body)
{
	return INVOKE(0x20E1DA, 0x0, screen_error_ok_dialog_with_custom_text, channel_type, ui_error_index, window_index, user_flags, ok_callback, fallback, custom_title, custom_body);
}

void __cdecl user_interface_error_display_ok_cancel_dialog_with_ok_callback(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, uint16 user_flags, void* ok_callback_handle, e_ui_error_types error_type)
{
	INVOKE(0x20E3BB, 0x0, user_interface_error_display_ok_cancel_dialog_with_ok_callback, channel_type, window_index, user_flags, ok_callback_handle, error_type);
	return;
}

void __cdecl user_interface_back_out_from_channel(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index)
{
	INVOKE(0x2096DA, 0x0, user_interface_back_out_from_channel, channel_type, window_index);
	return;
}

bool __cdecl user_interface_back_out_from_channel_by_id(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, e_user_interface_screen_id id)
{
	return INVOKE(0x209850, 0x0, user_interface_back_out_from_channel_by_id, channel_type, window_index, id);
}

void __cdecl user_interface_enter_game_shell(int32 context)
{
	INVOKE(0x20CE70, 0x0, user_interface_enter_game_shell, context);
	return;
}

void __cdecl render_menu_user_interface(int32 controller_index, e_user_interface_render_window render_window, rectangle2d* out_rect2d)
{
	INVOKE(0x20B697, 0x0, render_menu_user_interface, controller_index, render_window, out_rect2d);
	return;
}

void __cdecl user_interface_return_to_mainmenu(bool a1)
{
	INVOKE(0x20A87E, 0x0, user_interface_return_to_mainmenu, a1);
	return;
}

void __cdecl user_interface_update(real32 dt)
{
	static real32 user_interface_precise_accumulator_msec = 0.f;
	user_interface_precise_accumulator_msec += (dt * 1000.f) - floor(dt * 1000.f);
	if (user_interface_precise_accumulator_msec >= 1.f)
	{
		*Memory::GetAddress<int32*>(0x971900, 0x0) += (int32)user_interface_precise_accumulator_msec;
		user_interface_precise_accumulator_msec -= (int32)user_interface_precise_accumulator_msec;
	}

	INVOKE(0x20CA7D, 0x0, user_interface_update, dt);
	return;
}

uint32 user_interface_set_context_presence(e_context_presence game_mode)
{
	return XUserSetContext(0, X_CONTEXT_PRESENCE, game_mode);
}
