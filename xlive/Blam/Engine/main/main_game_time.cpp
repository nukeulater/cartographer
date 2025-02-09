#include "stdafx.h"
#include "main_game_time.h"

#include "game/game.h"
#include "game/game_time.h"
#include "math/math.h"
#include "shell/shell_windows.h"
#include "game/player_control.h"

bool g_main_game_time_frame_limiter_enabled = false;
LARGE_INTEGER g_main_game_time_counter_last_time;

s_main_time_debug g_main_game_time_debug;

real32 main_time_get_max_frame_time()
{
	time_globals* time_globals = time_globals::get();
	real32 result = time_globals->tick_length - (real32)(time_globals->game_ticks_leftover / (real32)time_globals->ticks_per_second);
	return MAX(result, 0.0f);
}

void __cdecl compute_target_tick_count(float dt, float* out_time_delta, int* out_target_tick_count)
{
	typedef void(__cdecl* compute_target_tick_count_t)(float, float*, int*);
	auto p_compute_target_tick_count = Memory::GetAddress<compute_target_tick_count_t>(0x7C1BF);

	typedef bool(__cdecl* game_is_not_paused_t)();
	auto p_game_is_not_paused = Memory::GetAddress<game_is_not_paused_t>(0x497EA);

	p_compute_target_tick_count(dt, out_time_delta, out_target_tick_count);
}

void __cdecl main_game_time_initialize_hook()
{
	// windows 10 version 2004 and above added behaviour changes to how windows timer resolution works, and we have to explicitly set the time resolution
	// and since they were added, when playing on a laptop on battery it migth add heavy stuttering when using a frame limiter based on Sleep function (or std::this_thread::sleep_for) implementation
	// the game sets them already but only during the loading screen period, then it resets to system default when the loading screen ends 
	// (tho i think in the new implementation is working on a per thread basis now instead of global frequency, since it still works even when the game resets after loading screen ends and loading screen runs in another thread)

	// More code in Tweaks.cpp in InitH2Tweaks

	// More details @ https://randomascii.wordpress.com/2020/10/04/windows-timer-resolution-the-great-rule-change/

	timeBeginPeriod(SYSTEM_TIMER_RESOLUTION_MS);
	g_main_game_time_counter_last_time = shell_time_counter_now(NULL);
	INVOKE(0x2869F, 0x24841, main_game_time_initialize_hook);
}

uint64 __cdecl main_time_get_absolute_milliseconds()
{
	uint64 milliseconds = system_milliseconds();
	s_main_time_globals* main_time_globals = s_main_time_globals::get();

	// copy the high part
	milliseconds |= (uint64)(main_time_globals->last_milliseconds >> 32) << 32;

	if (milliseconds < main_time_globals->last_milliseconds)
	{
		// adjust the time, a complete cycle of system_milliseconds has passed
		milliseconds += UINT_MAX + 1ULL;
	}

	ASSERT(milliseconds >= main_time_globals->last_milliseconds);
	return milliseconds;
}

typedef void(__cdecl* t_main_time_reset)();
t_main_time_reset p_main_time_reset;

void __cdecl main_time_reset_hook()
{
	s_main_time_globals* main_time_globals = s_main_time_globals::get();
	main_time_globals->last_milliseconds = system_milliseconds();
	g_main_game_time_counter_last_time = shell_time_counter_now(NULL);
	// main_time_globals->should_reset = true;
}

static real32 main_time_get_delta_sec_precise(LARGE_INTEGER counter_now, LARGE_INTEGER freq)
{
	real32 result = ((real32)shell_time_counter_diff(counter_now, g_main_game_time_counter_last_time).QuadPart) / (real32)freq.QuadPart;
	return result;
}

real32 __cdecl main_time_update_hook(bool fixed_time_step, real32 fixed_time_delta)
{
	/*typedef real32(__cdecl* main_time_update_t)(bool, real32);
	auto p_main_time_update = Memory::GetAddress<main_time_update_t>(0x28814);*/

	typedef void(__cdecl* t_shell_update)();
	auto p_shell_update = Memory::GetAddress<t_shell_update>(0x7902, 0xBA18);

	int game_time;
	real32 dt_sec = 0.0f;

	LARGE_INTEGER freq;
	freq = shell_time_counter_freq();

	uint32 time_now_msec;

	s_main_time_globals* main_time_globals = s_main_time_globals::get();
	game_time = game_in_progress() ? get_game_time_ticks() : 0;

	bool use_precise_counters = true;

	// TranslateMessage()
	// TODO move to function and cleanup
	p_shell_update();

	if (fixed_time_step)
	{
		dt_sec = fixed_time_delta;
		if (time_globals::available())
			time_globals::get()->game_ticks_leftover = 0.0f;
	}
	else
	{
		if (use_precise_counters)
		{
			dt_sec = main_time_get_delta_sec_precise(shell_time_counter_now(NULL), freq);
		}
		else
		{
			time_now_msec = main_time_get_absolute_milliseconds();
			dt_sec = (real32)((real64)(time_now_msec - main_time_globals->last_milliseconds) / 1000.f);
		}

		// don't run the frame limiter when time step is fixed, because the code doesn't support it
		// in case of fixed time step, frame limiter should be handled by the other frame limiter
		if (g_main_game_time_frame_limiter_enabled || game_is_minimized())
		{
			if (time_globals::available())
			{
				// if there's game tick leftover time (i.e the actual game tick update executed faster than the actual engine's fixed time step)
				// FIXED by interpolation: 
				// limit the framerate to get back in sync with the renderer to prevent ghosting and jagged movement
				const real32 desired_frame_time = main_time_get_max_frame_time();
				while (dt_sec < desired_frame_time)
				{
					uint32 yield_time_msec = 0;
					real32 fMsSleep = (real32)(desired_frame_time - dt_sec) * 1000.f;

					if (fMsSleep >= 1.0f)
					{
						yield_time_msec = (int32)fMsSleep;

						// TODO FIXME to reduce stuttering, spend some of the time to sleep by CPU spinning,
						// Sleep is not precise since Windows is not a RTOS
						Sleep(yield_time_msec);
					}

					if (use_precise_counters)
					{
						dt_sec = main_time_get_delta_sec_precise(shell_time_counter_now(NULL), freq);
					}
					else
					{
						time_now_msec = main_time_get_absolute_milliseconds();
						dt_sec = (real32)((real64)(time_now_msec - main_time_globals->last_milliseconds) / 1000.f);
					}
				}
			}
			else
			{
				Sleep(15u);

				if (use_precise_counters)
				{
					dt_sec = main_time_get_delta_sec_precise(shell_time_counter_now(NULL), freq);
				}
				else
				{
					time_now_msec = main_time_get_absolute_milliseconds();
					dt_sec = (real32)((real64)(time_now_msec - main_time_globals->last_milliseconds) / 1000.f);
				}
			}
		}
	}

	if (use_precise_counters)
	{
		time_now_msec = main_time_get_absolute_milliseconds();
		g_main_game_time_debug.dt_default = (real32)((real64)(time_now_msec - main_time_globals->last_milliseconds) / 1000.f);
		g_main_game_time_debug.dt_performance_counter = dt_sec;
	}
	else
	{
		g_main_game_time_debug.dt_default = dt_sec;
		g_main_game_time_debug.dt_performance_counter = main_time_get_delta_sec_precise(shell_time_counter_now(NULL), freq);
	}

	g_main_game_time_counter_last_time = shell_time_counter_now(NULL);

	dt_sec = PIN(dt_sec, 0.0f, 10.f);
	main_time_globals->last_milliseconds = main_time_get_absolute_milliseconds();
	main_time_globals->last_game_time = game_time;
	main_time_globals->field_16[0] = *Memory::GetAddress<__int64*>(0xA3E440);
	main_time_globals->field_16[1] = *Memory::GetAddress<__int64*>(0xA3E440);

	player_control_update_dt(dt_sec);

	//LOG_TRACE_GAME("main_time_update() - timeDeltaSeconds: {}", timeDeltaSeconds);
	return dt_sec;
}

void main_game_time_apply_patches()
{
	PatchCall(Memory::GetAddress(0x39BE3, 0xC03E), main_time_update_hook);
	PatchCall(Memory::GetAddress(0x39E3D, 0xBA40), main_game_time_initialize_hook);

	DETOUR_ATTACH(p_main_time_reset, Memory::GetAddress<t_main_time_reset>(0x286C5, 0x24867), main_time_reset_hook);

	if (!Memory::IsDedicatedServer())
	{
		PatchCall(Memory::GetAddress(0x39C0D), main_time_update_hook);
		//NopFill(Memory::GetAddress(0x2728E7), 5);

		//PatchCall(Memory::GetAddress(0x39D04), compute_target_tick_count);

		//NopFill(Memory::GetAddress(0x39BDA), 2);
		//NopFill(Memory::GetAddress(0x39DF0), 8);

		//NopFill(Memory::GetAddress(0x39DE1), 5);
	}
}

