#include "stdafx.h"
#include "screen_error_dialog.h"
#include "interface/user_interface.h"
#include "interface/user_interface_controller.h"


void* c_screen_error_dialog_ok::load_for_active_users(s_screen_parameters* parameters)
{
	parameters->user_flags = user_interface_controller_get_signed_in_controllers_mask();
	return INVOKE(0x20E032, 0x0, c_screen_error_dialog_ok::load_for_active_users, parameters);
}

void c_screen_error_dialog_ok::apply_patches()
{
	WritePointer(Memory::GetAddress(0x20E173) + 1, c_screen_error_dialog_ok::load_for_active_users);
}


void* c_screen_error_dialog_ok_cancel::load_for_active_users(s_screen_parameters* parameters)
{
	parameters->user_flags = user_interface_controller_get_signed_in_controllers_mask();
	return INVOKE(0x20E0C0, 0x0, c_screen_error_dialog_ok_cancel::load_for_active_users, parameters);
}


void c_screen_error_dialog_ok_cancel::apply_patches()
{
	WritePointer(Memory::GetAddress(0x20E2F9) + 1, c_screen_error_dialog_ok_cancel::load_for_active_users);
	WritePointer(Memory::GetAddress(0x20E25C) + 1, c_screen_error_dialog_ok_cancel::load_for_active_users);
	
}
