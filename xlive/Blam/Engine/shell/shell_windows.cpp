#include "stdafx.h"
#include "shell_windows.h"

#include "shell.h"
#include "shell_windows_internals.h"

#include "cseries/cseries_errors.h"
#include "main/main.h"
#include "math/math.h"

#include "H2MOD/Modules/Shell/H2MODShell.h"
#include "H2MOD/Utils/Utils.h"

static LARGE_INTEGER g_startup_counter;
static DWORD(WINAPI* p_timeGetTime)() = timeGetTime;

DWORD WINAPI timeGetTime_hook();

// WinMain replacement
int WINAPI H2WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);

// Destroys global windows used by the game
void destroy_windows(void);

void __cdecl show_fatal_error(int32 error_id);

bool __cdecl pcc_get_properties(void);

LRESULT WINAPI H2WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int32* global_window_bounds_x_get(void)
{
	return Memory::GetAddress<int32*>(0xA3E408);
}

int32* global_window_bounds_y_get(void)
{
	return Memory::GetAddress<int32*>(0xA3E40C);
}

void shell_windows_apply_patches(void)
{
	DETOUR_ATTACH(p_timeGetTime, timeGetTime, timeGetTime_hook);
	WriteJmpTo(Memory::GetAddress(0x7E43), H2WinMain);
	return;
}

bool __cdecl game_is_minimized(void)
{
	return INVOKE(0x28729, 0x248AB, game_is_minimized);
}

LARGE_INTEGER shell_get_startup_counter()
{
	return g_startup_counter;
}

unsigned long long shell_time_from_counter(LARGE_INTEGER counter, LARGE_INTEGER freq, unsigned long long denominator)
{
	unsigned long long _Whole, _Part;

	_Whole = (counter.QuadPart / freq.QuadPart) * denominator;
	_Part = (counter.QuadPart % freq.QuadPart) * denominator / freq.QuadPart;

	return _Whole + _Part;
}

LARGE_INTEGER shell_time_counter_now(LARGE_INTEGER* freq)
{
	LARGE_INTEGER counter;
	if (freq) 
	{
		QueryPerformanceFrequency(freq);
	}
	QueryPerformanceCounter(&counter);
	counter.QuadPart -= shell_get_startup_counter().QuadPart;
	return counter;
}

unsigned long long shell_time_diff(LARGE_INTEGER t2, unsigned long long denominator)
{
	LARGE_INTEGER counter, freq;
	counter = shell_time_counter_now(&freq);
	counter.QuadPart -= t2.QuadPart;
	return shell_time_from_counter(counter, freq, denominator);
}

unsigned long long shell_time_now(unsigned long long denominator)
{
	LARGE_INTEGER counter, freq;
	counter = shell_time_counter_now(&freq);
	return shell_time_from_counter(counter, freq, denominator) + (k_process_system_time_startup_offset_sec * denominator);
}

unsigned long long shell_time_now_sec()
{
	return shell_time_now(k_shell_time_sec_denominator);
}

unsigned long long shell_time_now_msec()
{
	return shell_time_now(k_shell_time_msec_denominator);
}

uint32 __cdecl system_milliseconds()
{
	return INVOKE(0x37E51, 0x2B4CE, system_milliseconds);
}

DWORD WINAPI timeGetTime_hook()
{
	unsigned long long time_now_msec = shell_time_now_msec();
	return (DWORD)time_now_msec;
}
static_assert(std::is_same_v<decltype(timeGetTime), decltype(timeGetTime_hook)>, "Invalid timeGetTime_hook signature");

void shell_windows_initialize()
{
	QueryPerformanceCounter(&g_startup_counter);
}

static void shell_system_set_timer_resolution_max(bool enable)
{
	ULONG ulMinimumResolution, ulMaximumResolution, ulCurrentResolution;
	NtQueryTimerResolutionHelper(&ulMinimumResolution, &ulMaximumResolution, &ulCurrentResolution);
	NtSetTimerResolutionHelper(ulMaximumResolution, enable, &ulCurrentResolution);
}

void shell_windows_throttle_framerate(int desired_framerate)
{
	static LARGE_INTEGER last_counter;
	static int last_desired_framerate_setting = -1;
	static bool frame_limiter_initialized = false;

	const int threadWaitTimePercentage = 90;
	static HANDLE hFrameLimitTimer = NULL;

	if (desired_framerate <= 0)
	{
		last_desired_framerate_setting = MAX(desired_framerate, 15);
		frame_limiter_initialized = false;
		return;
	}

	if (last_desired_framerate_setting != desired_framerate)
	{
		last_desired_framerate_setting = desired_framerate;
		frame_limiter_initialized = false;
	}

	if (!frame_limiter_initialized)
	{
		last_counter = shell_time_counter_now(NULL);
		frame_limiter_initialized = true;

		if (NULL == hFrameLimitTimer)
		{
			hFrameLimitTimer = CreateWaitableTimer(NULL, FALSE, NULL);

			atexit([]() {
				if (NULL != hFrameLimitTimer)
					CloseHandle(hFrameLimitTimer);

				// reset timer resolution back to default on exit
				shell_system_set_timer_resolution_max(false);
				});
		}

		// skip the first frame after init
		return;
	}

	auto min_frametime_usec = (long long)(1000000.0 / (double)desired_framerate);
	auto dt_usec = shell_time_diff(last_counter, k_shell_time_usec_denominator);

	const int min_time_to_suspend_exec_usec = 3000;

	if (dt_usec < min_frametime_usec)
	{
		auto sleep_time_usec = min_frametime_usec - dt_usec;

		// sleep threadWaitTimePercentage out of the target render time using thread sleep or timer wait
		long long system_yield_time_usec = (threadWaitTimePercentage * sleep_time_usec) / 100;

		// sleep just the milliseconds part
		// system_yield_time_usec = system_yield_time_usec - (system_yield_time_usec % 1000);

		// skip CPU yield if time is lower than 3ms
		// because the system timer isn't precise enough for our needs
		if (system_yield_time_usec > min_time_to_suspend_exec_usec)
		{
			if (NULL != hFrameLimitTimer)
			{
				// Create an unnamed waitable timer.
				ULONG ulMinimumResolution, ulMaximumResolution, ulCurrentResolution;
				NtQueryTimerResolutionHelper(&ulMinimumResolution, &ulMaximumResolution, &ulCurrentResolution);

				if (10ll * system_yield_time_usec > ulMaximumResolution)
				{
					LARGE_INTEGER liDueTime;
					NtSetTimerResolutionHelper(ulMaximumResolution, TRUE, &ulCurrentResolution);

					liDueTime.QuadPart = -10ll * system_yield_time_usec;
					if (SetWaitableTimer(hFrameLimitTimer, &liDueTime, 0, NULL, NULL, TRUE))
					{
						// Wait for the timer.
						NtWaitForSingleObjectHelper(hFrameLimitTimer, FALSE, &liDueTime);
					}
				}
			}

			/*int sleepTimeMs = system_yield_time_usec / 1000ll;
			if (sleepTimeMs >= 0)
				Sleep(sleepTimeMs);*/
		}

		// spin-lock the remaining slice of time
		while (true)
		{
			if (shell_time_diff(last_counter, k_shell_time_usec_denominator) >= min_frametime_usec)
				break;
		}
	}

	last_counter = shell_time_counter_now(NULL);
}

int32 fatal_error_id_get(void)
{
	return *Memory::GetAddress<int*>(0x46DAD4);
}

int WINAPI H2WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// set args
	WriteValue(Memory::GetAddress(0x46D9BC), lpCmdLine); // command_line_args
	WriteValue(Memory::GetAddress(0x46D9C0), hInstance); // g_instance
	WriteValue(Memory::GetAddress(0x46D9CC), nShowCmd); // g_CmdShow

	// window setup
	wcscpy_s(Memory::GetAddress<wchar_t*>(0x46D9D4), 0x40, L"halo"); // ClassName
	wcscpy_s(Memory::GetAddress<wchar_t*>(0x46DA54), 0x40, L"Halo 2 - Project Cartographer"); // WindowName

	WriteValue(Memory::GetAddress(0x46D9D0), H2WndProc); // g_WndProc_ptr

	bool pcc_result = pcc_get_properties();
	if (!pcc_result)
	{
		error(2, "Failed to get PCC info / insufficient system resources");
		ASSERT(pcc_result == true);
		
		// ### TODO change this?
		std::thread([]() {
			_Shell::OpenMessageBox(NULL, MB_ICONEXCLAMATION, "PCC Error", "Failed to get compatibility info.");
		}).detach();

		show_fatal_error(108);
	}

	// mouse cursor setup
	HCURSOR cursor = LOG_CHECK(LoadCursor(NULL, MAKEINTRESOURCE(0x7F00)));
	WriteValue(Memory::GetAddress(0x46D9B8), cursor); // g_hCursor

	// mess around with xlive (not calling XLiveInitialize etc)
	WriteValue<BYTE>(Memory::GetAddress(0x4FAD98), 1);


	// intialize some basic game subsystems
	if (shell_initialize())
	{
		main_loop();		// actually run game
		shell_dispose();	// cleanup
	}
	else if (int32 g_fatal_error_id = fatal_error_id_get(); g_fatal_error_id)
	{
		error(2, "shell_initialize failed!");
		show_fatal_error(g_fatal_error_id);
	}

	int result;
	int32 g_fatal_error_id = fatal_error_id_get();
	if (g_fatal_error_id)	// check if the game exited cleanly
	{
		destroy_windows();
		show_fatal_error(g_fatal_error_id);
		result = 1;
	}
	else
	{
		result = 0;
	}

	return result;
}

void destroy_windows(void)
{
	HWND hWnd = *Memory::GetAddress<HWND*>(0x46D9C4);
	HWND d3d_window = *Memory::GetAddress<HWND*>(0x46D9C8); // not sure what this window is actual for, used in IDirect3DDevice9::Present
	if (hWnd)
	{
		DestroyWindow(hWnd);
	}

	if (d3d_window)
	{
		DestroyWindow(d3d_window);
	}

	return;
}

void __cdecl show_fatal_error(int32 error_id)
{
	error(2, "error_id: %d", error_id);
	INVOKE(0x4A2E, 0x0, show_fatal_error, error_id);
	return;
}

bool __cdecl pcc_get_properties(void)
{
	return INVOKE(0x260DDD, 0x0, pcc_get_properties);
}

bool __cdecl gfwl_gamestore_initialize(void)
{
	return INVOKE(0x202F3E, 0x0, gfwl_gamestore_initialize);
}

LRESULT WINAPI H2WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static FrequencyLimiter frqLimiter(150);
	bool* window_in_focus = Memory::GetAddress<bool*>(0x46DAD9);
	bool* unk_condition1 = Memory::GetAddress<bool*>(0x46DAD8);
	WNDPROC g_WndProc = Memory::GetAddress<WNDPROC>(0x790E);

	LRESULT result;
	if (uMsg != WM_SETCURSOR)
	{
		result = g_WndProc(hWnd, uMsg, wParam, lParam);
	}
	else
	{
		// if we have to set the cursor, limit the frequency of the cursor being set
		// because it's very heavy on the CPU
		bool enable_cursor = *window_in_focus && !*unk_condition1;
		// if the cursor is about to get disabled, reset the frqLimiter
		// and allow the cursor to be disabled
		if (!enable_cursor)
		{
			frqLimiter.Reset();
		}

		result = 1;

		// ShouldUpdate also updates the state of the frqLimiter
		if (frqLimiter.ShouldUpdate())
		{
			result = g_WndProc(hWnd, uMsg, wParam, lParam);
		}
	}

	return result;
}