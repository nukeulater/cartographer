#pragma once

#include "Blam/Engine/Players/LocalPlayers.h"

#define GAME_MAX_INPUT_DEVICES 20

enum // controller button inputs
{
	_controller_input_dpad_up,
	_controller_input_dpad_down,
	_controller_input_dpad_left,
	_controller_input_dpad_right,
	_controller_input_start,
	_controller_input_back,
	_controller_input_left_thumb,
	_controller_input_right_thumb,
	_controller_input_left_shoulder,
	_controller_input_right_shoulder,
	_controller_input_a,
	_controller_input_b,
	_controller_input_x,
	_controller_input_y,
	_controller_input_end
};

enum e_deadzone_types
{
	_controller_input_thumbstick_deadzone_axial,
	_controller_input_thumbstick_deadzone_radial,
	_controller_input_thumbstick_deadzone_both,
};

struct s_controller_thumbsticks_points
{
	short thumbstickLX;
	short thumbstickLY;
	short thumbstickRX;
	short thumbstickRY;
};

struct s_controller_input_settings
{
	// in case more settings will be applied
	int settings_version;

	int deadzone_type[ENGINE_MAX_LOCAL_PLAYERS];
	short deadzone_radial[ENGINE_MAX_LOCAL_PLAYERS];
	s_controller_thumbsticks_points deadzone_axial[ENGINE_MAX_LOCAL_PLAYERS];
	WORD button_layout[ENGINE_MAX_LOCAL_PLAYERS][_controller_input_end];
	float sensitivity[ENGINE_MAX_LOCAL_PLAYERS];

	void GetLayout(WORD* out)
	{
		memcpy(out, button_layout, sizeof(button_layout));
	}

	void FromString(std::string string)
	{
		std::string item;
		std::stringstream ss(string);

		for (int i = 0; i < _controller_input_end; i++)
		{
			if (std::getline(ss, item, '-'))
			{
				button_layout[i] = std::stoi(item);
			}
		}
	}

	static short DeadzoneFromPercentage(float deadzone)
	{
		return (short)(32767.f * (deadzone / 100.f));
	}

	void SetDefaultSettings()
	{
		const short default_radial_deadzone = 0;
		const float default_sensitivity = 3.0f;
		const e_deadzone_types default_deadzone_type = _controller_input_thumbstick_deadzone_axial;
		const s_controller_thumbsticks_points default_deadzones =
		{
			XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
			XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
			XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
			XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
		};

		for (int i = 0; i < ENGINE_MAX_LOCAL_PLAYERS; i++)
		{
			deadzone_type[i] = default_deadzone_type;
			deadzone_axial[i] = default_deadzones;
			deadzone_radial[i] = default_radial_deadzone;
			sensitivity[i] = default_sensitivity;
		}
	}
};

namespace ControllerInput
{
	// this is actually an interface
	// which is inherited by 2 classes, c_xinput_device for controllers
	// and c_dinput_device, for other peripherals
	class alignas(4) c_xinput_device
	{
	public:
		void **vtbl;

		DWORD __thiscall get_device_state(XINPUT_STATE* out_state);
	};
	static_assert(sizeof(c_xinput_device) == 4, "invalid c_input_device size");

	void* __cdecl get_controller_input(__int16 index);
	void __cdecl abstraction_update();

	void SetSensitiviy(float value);
	bool HasInput();
	void Initialize();
	void SetDeadzones(e_deadzone_types deadzone_type, float deadzoneLX, float deadzoneLY, float deadzoneRX, float deadzoneRY, float deadzoneRadial);
	void ApplyDefaultSettings();
}