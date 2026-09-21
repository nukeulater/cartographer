#pragma once

/* constants */

enum
{
	k_process_system_time_startup_offset_sec = (1 * 60 * 60),	// 1 hour offset
	k_shell_time_sec_denominator = 1,
	k_shell_time_msec_denominator = 1000,
	k_shell_time_usec_denominator = 1000000
};

/* structures */

struct s_window_globals
{
	HCURSOR cursor;
	LPSTR lpCmdLine;
	HINSTANCE hInstance;
	HWND hWnd;
	HWND hWndPresentTarget;
	int32 show_cmd;
	WNDPROC wnd_proc;
	wchar_t class_name[64];
	wchar_t window_name[64];
};

/* globals */

/* prototypes */

uint32 shell_get_instance_num();

s_window_globals* window_globals_get(void);

bool shell_platform_initialize(void);

bool* should_initilize_xlive_get(void);

bool* xlive_initilized_get(void);

int32* fatal_error_id_get(void);

void shell_windows_apply_patches();

void shell_windows_initialize();

uint32 __cdecl system_milliseconds(void);

uint64 shell_time_counter_freq();

uint64 shell_time_counter_now();

uint64 shell_time_now_sec();

uint64 shell_time_now_msec();

uint64 shell_time_now(uint64 denominator);

void shell_windows_throttle_framerate(uint64 last_counter, int32 desired_framerate);

bool __cdecl gfwl_gamestore_initialize(void);

uint32 shell_windows_get_monitor_index(void);

void shell_windows_calculate_instance_num(void);
