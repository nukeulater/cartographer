#include "stdafx.h"
#include "input_process.h"
#include "interface/screens/screen_xbox_live_task_progress_dialog.h"
#include "input_windows.h"

void input_split_task_progress_callback(c_screen_xbox_live_task_progress_dialog* dialog)
{
	if (input_windows_has_split_device_active())
		dialog->set_display_text_raw(L"Splitting in progress, please wait...");
	else
		dialog->set_display_text_raw(L"Restoring inputs, please wait...");

	//	update the delay_timer in progress screen update
	input_device_change_delay_timer++;

	if (!input_windows_processing_device_change())
	{
		dialog->close_task();
	}
}