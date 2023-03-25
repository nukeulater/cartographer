#include "stdafx.h"
#include "ControllerInput.h"

#include "H2MOD\Engine\Engine.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "Util\Hooks\Hook.h"

namespace ControllerInput
{
	namespace {
		int* main_controller_index;

		short radial_deadzone[ENGINE_MAX_LOCAL_PLAYERS];
		s_controller_thumbsticks_points axial_deadzones[ENGINE_MAX_LOCAL_PLAYERS];

		typedef void(__cdecl* sub_B31EA2_t)(int local_player_index, BYTE* keybind_base, void* a4, float* a5, float* a6, BYTE* a7);
		sub_B31EA2_t p_sub_B31EA2;

		byte* default_profile;
	}

	const short default_radial_deadzone = 0;
	const s_controller_thumbsticks_points default_deadzones = 
	{ 
		XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
		XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
		XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
		XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
	};

	DWORD __thiscall c_xinput_device::get_device_state(XINPUT_STATE* out_state)
	{
		auto p_get_device_state = Memory::GetAddress<DWORD(__thiscall*)(c_xinput_device*, XINPUT_STATE*)>(0x0);
		DWORD api_error = p_get_device_state(this, out_state);
		if (api_error == 0)
		{
			if (Engine::get_game_life_cycle() == _life_cycle_in_game 
				|| Engine::get_current_engine_type() == _single_player)
			{
				// apply button layout re-map
				WORD layout_controller_input = 0;
				for (int i = 0; i < _controller_input_end; i++)
				{
					WORD buttonFlags = out_state->Gamepad.wButtons;
					if ((buttonFlags & (1 << i)) != 0)
					{
						layout_controller_input |= H2Config_CustomLayout.button_layout[i];
					}
				}
				out_state->Gamepad.wButtons = layout_controller_input;
			}
		}

		return api_error;
	}
	void __declspec(naked) get_device_state_hook(void) { __asm jmp c_xinput_device::get_device_state };

	typedef void(__cdecl* user_profile_get_properties_t)(int, void*);
	user_profile_get_properties_t p_user_profile_properties_get;

	void __cdecl user_profile_properties_get_hook(int controller_index, void* out_properties)
	{
		p_user_profile_properties_get(controller_index, out_properties);
		s_controller_thumbsticks_points* profile_deadzones = (s_controller_thumbsticks_points*)((BYTE*)out_properties + 5752);
		// if (modified_aiming_enabled) ?
		{
			*profile_deadzones = axial_deadzones[controller_index];
		}
	}

	float normalize_thumbstick_point(float thumbstick_point, float max_point)
	{
		float normalized_point = thumbstick_point / max_point;
		normalized_point = fminf(fmaxf(normalized_point, -1.0f), 1.0f);
		return normalized_point;
	}

	// TODO: Refactor this to a General Input namespace, it's called whether or not you are using a controller.
	void __cdecl ControllerInput::abstraction_update()
	{
		int user_index = 0;
		float thumbstickR[2], thumbstickL[2];
		thumbstickR[0] = thumbstickR[1] = 0;
		thumbstickL[0] = thumbstickL[1] = 0;

		auto p_input_abstraction_update = Memory::GetAddress<void(__cdecl*)()>(0x628A8);
		short* controller_input = (short*)ControllerInput::get_controller_input(user_index);
		BYTE* unk_input_data = Memory::GetAddress<BYTE*>(0x4AE578);

		s_controller_thumbsticks_points* controller_thunbstick_positions = (s_controller_thumbsticks_points*)((BYTE*)controller_input + 52);

		// axial deadzones are handled by game's code
		// 
		if (radial_deadzone > 0)
		{
			// adjust the controller thumbsticks if radial deadzones are enabled
			float deadzone_magnitude_squared = radial_deadzone[user_index] * radial_deadzone[user_index];
			float left_thumbstick_magnitude_squared 
				= controller_thunbstick_positions->thumbstickLX * controller_thunbstick_positions->thumbstickLX;
			left_thumbstick_magnitude_squared += 
				controller_thunbstick_positions->thumbstickLY * controller_thunbstick_positions->thumbstickLY;

			float right_thumbstick_magnitude_squared
				= controller_thunbstick_positions->thumbstickRX * controller_thunbstick_positions->thumbstickRX;
			right_thumbstick_magnitude_squared *= controller_thunbstick_positions->thumbstickRY * controller_thunbstick_positions->thumbstickRY;

			if (deadzone_magnitude_squared >= left_thumbstick_magnitude_squared)
			{
				controller_thunbstick_positions->thumbstickLX = 0;
				controller_thunbstick_positions->thumbstickLY = 0;
			}

			if (deadzone_magnitude_squared >= right_thumbstick_magnitude_squared)
			{
				controller_thunbstick_positions->thumbstickRX = 0;
				controller_thunbstick_positions->thumbstickRY = 0;
			}
		}

		if (!H2Config_controller_modern)
		{
			// stock input handler
			p_input_abstraction_update();
		}
		else 		
		{
			thumbstickL[0] = normalize_thumbstick_point(controller_thunbstick_positions->thumbstickLX, 32767.0f);
			thumbstickL[1] = normalize_thumbstick_point(controller_thunbstick_positions->thumbstickLY, 32767.0f);
			thumbstickR[0] = normalize_thumbstick_point(controller_thunbstick_positions->thumbstickRX, 32767.0f);
			thumbstickR[1] = normalize_thumbstick_point(controller_thunbstick_positions->thumbstickRY, 32767.0f);

			p_sub_B31EA2(user_index, default_profile, controller_input, thumbstickL, thumbstickR, unk_input_data);
		}
	}

	void* ControllerInput::get_controller_input(__int16 index)
	{
		typedef void* (__cdecl* get_controller_input_t)(__int16);
		auto p_get_controller_input = Memory::GetAddress<get_controller_input_t>(0x2F433);
		return p_get_controller_input(index);
	}

	void ControllerInput::SetSensitiviy(float value)
	{
		if (Memory::IsDedicatedServer()) return;
		if (value == 0.0f) return;

		value = blam_max(value - 1.0f, 0.0f);

		*Memory::GetAddress<float*>(0x4A89B8) = 80.0f + 20.0f * value; //x-axis
		*Memory::GetAddress<float*>(0x4A89BC) = 40.0f + 10.0f * value; //y-axis
	}

	void ControllerInput::SetDeadzones(e_deadzone_types deadzone_type, float deadzoneLX, float deadzoneLY, float deadzoneRX, float deadzoneRY, float deadzoneRadial)
	{
		if (Memory::IsDedicatedServer()) return;

		if (deadzone_type == _controller_input_thumbstick_deadzone_axial
			|| deadzone_type == _controller_input_thumbstick_deadzone_both) 
		{
			for (int i = 0; i < ENGINE_MAX_LOCAL_PLAYERS; i++)
			{
				axial_deadzones[i].thumbstickLX = (short)((float)MAXSHORT * (deadzoneLX / 100.f));
				axial_deadzones[i].thumbstickLX = (short)((float)MAXSHORT * (deadzoneLY / 100.f));
				axial_deadzones[i].thumbstickRX = (short)((float)MAXSHORT * (deadzoneRX / 100.f));
				axial_deadzones[i].thumbstickRY = (short)((float)MAXSHORT * (deadzoneRY / 100.f));
			}

		}
		else
		{
			for (int i = 0; i < ENGINE_MAX_LOCAL_PLAYERS; i++)
			{
				axial_deadzones[i] = { 0, 0, 0, 0 };
			}
		}

		if (deadzone_type == _controller_input_thumbstick_deadzone_radial 
			|| deadzone_type == _controller_input_thumbstick_deadzone_both) 
		{
			for (int i = 0; i < ENGINE_MAX_LOCAL_PLAYERS; i++)
				radial_deadzone[i] = (short)((float)MAXSHORT * (deadzoneRadial / 100.f));
		} 
		else
		{
			for (int i = 0; i < ENGINE_MAX_LOCAL_PLAYERS; i++)
				radial_deadzone[i] = 0;
		}
	}

	bool ControllerInput::HasInput()
	{
		BYTE* controller_input = (BYTE*)ControllerInput::get_controller_input(0);
		short* scInput = (short*)controller_input;

		bool buttons = false;
		for (int i = 0; i < 42; i++)
		{
			if (controller_input[i] != 0 && controller_input[i] != 0xff && controller_input[i] != 0x40)
				buttons = true;
		}

		int thumbStickL = 0;
		int thumbStickR = 0;
		bool radialL = false;
		bool radialR = false;

		if (H2Config_Controller_Deadzone == _controller_input_thumbstick_deadzone_axial 
			|| H2Config_Controller_Deadzone == _controller_input_thumbstick_deadzone_both)
		{
			if (abs(scInput[26]) >= ((float)MAXSHORT * (H2Config_Deadzone_A_X / 100.f)))
				thumbStickL++;
			if (abs(scInput[27]) >= ((float)MAXSHORT * (H2Config_Deadzone_A_Y / 100.f)))
				thumbStickL++;
			if (abs(scInput[28]) >= ((float)MAXSHORT * (H2Config_Deadzone_A_X / 100.f)))
				thumbStickR++;
			if (abs(scInput[29]) >= ((float)MAXSHORT * (H2Config_Deadzone_A_Y / 100.f)))
				thumbStickR++;
		}

		unsigned int ar = pow((short)((float)MAXSHORT * (H2Config_Deadzone_Radial / 100.f)), 2);
		unsigned int alx = pow(scInput[26], 2);
		unsigned int aly = pow(scInput[27], 2);
		unsigned int arx = pow(scInput[28], 2);
		unsigned int ary = pow(scInput[29], 2);
		unsigned int lh = alx + aly;
		unsigned int rh = arx + ary;
		if (rh >= ar)
			radialR = true;
		if (lh >= ar)
			radialL = true;
		return radialR || radialL || thumbStickL > 1 || thumbStickR > 1 || buttons;
	}

	void ControllerInput::Initialize()
	{
		if (Memory::IsDedicatedServer()) return;

		default_profile = Memory::GetAddress<BYTE*>(0x4a89b0);
		p_sub_B31EA2 = Memory::GetAddress<sub_B31EA2_t>(0x61EA2);
		
		//PatchCall(Memory::GetAddress(0x2e95d), sub_B31BF4);

		main_controller_index = Memory::GetAddress<int*>(0x47A714);
		//PatchCall(Memory::GetAddress(0x2FBD2), update_controller_input);
		PatchCall(Memory::GetAddress(0x39B82), ControllerInput::abstraction_update);
		WritePointer(Memory::GetAddress(0x3BBE0), get_device_state_hook);
		//Codecave(Memory::GetAddress(0x2e975), apply_dead_zones, 166);
		SetSensitiviy(H2Config_controller_sens);
		SetDeadzones(_controller_input_thumbstick_deadzone_axial, 
			default_deadzones.thumbstickLX, 
			default_deadzones.thumbstickLY, 
			default_deadzones.thumbstickRX, 
			default_deadzones.thumbstickRY, 
			0.0f
		);
	}
}