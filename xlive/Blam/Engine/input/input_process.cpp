#include "stdafx.h"
#include "input_process.h"
#include "input_windows.h"
#include "interface/screens/screen_xbox_live_task_progress_dialog.h"
#include "main/game_preferences.h"



/* constants */

const wchar_t* g_input_split_initiate_string[k_language_count]
{
	L"Splitting in progress, please wait...",
	L"分割中です。お待​​ちください...",
	L"Die Aufteilung läuft, bitte warten...",
	L"Fractionnement en cours, veuillez patienter...",
	L"División en progreso, por favor espere...",
	L"Divisione in corso, attendere prego...",
	L"분할하는 중입니다. 잠시 기다려 주세요...",
	L"正在分裂，请稍候...",
	L"Divisão em andamento, aguarde..."
};

const wchar_t* g_input_split_restore_string[k_language_count]
{
	L"Restoring inputs, please wait...",
	L"入力を復元しています。お待ちください...",
	L"Eingaben werden wiederhergestellt, bitte warten...",
	L"Restauration des entrées, veuillez patienter...",
	L"Restaurando entradas, por favor espere...",
	L"Ripristino degli input, attendere...",
	L"입력을 복원하는 중입니다. 잠시 기다려 주세요...",
	L"正在恢复输入，请稍候...",
	L"Restaurando entradas, aguarde..."
};


void input_split_task_progress_callback(c_screen_xbox_live_task_progress_dialog* dialog)
{
	e_language language = get_current_language();

	if (input_windows_has_split_device_active())
		dialog->set_display_text_raw((wchar_t*)g_input_split_initiate_string[language]);
	else
		dialog->set_display_text_raw((wchar_t*)g_input_split_restore_string[language]);

	//	update the delay_timer in progress screen update
	input_device_change_delay_timer++;

	if (!input_windows_processing_device_change())
	{
		dialog->close_task();
	}
}