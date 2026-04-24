#pragma once

#ifndef IMGUI_DISABLE

#include "input/controllers.h"
#include "saved_games/cartographer_player_profile/cartographer_player_profile.h"

/* constants */

/* enums */

enum e_network_stats_display_type : int32
{
	_network_stats_display_none,
	_network_stats_display_basic,
	_network_stats_display_complete,
};

enum e_imgui_window
{
	_imgui_window_weapon_offsets,
	_imgui_window_message_box,
	_imgui_window_advanced_settings,
	_imgui_window_console,
	k_imgui_window_count
};

/* globals */

extern s_saved_game_cartographer_player_profile* g_advanced_settings_current_cartographer_profile;
extern e_controller_index g_advanced_settings_current_controller_index;

namespace ImGuiHandler
{
	extern int g_network_stats_overlay;

	void Initalize(HWND hWnd);
	void DrawUpdate();

	bool InputIsCaptured();
	
	bool WindowIsActive(e_imgui_window window);
	void WindowToggle(e_imgui_window window);

	float WidthPercentage(float percent);
	void TextVerticalPad(const char* label);

	namespace ImMOTD {
		void Render(bool* p_open);
		void Open();
		void Close();
	}
	namespace ImAdvancedSettings
	{
		void Render(bool* p_open);
		void set_controller_index(e_controller_index controller_index);
		void Open();
		void Close();
	}
	namespace ImMessageBox
	{
		void Render(bool* p_open);
		void SetMessage(const char* message);
		void Open();
		void Close();
	}
	namespace WeaponOffsets
	{
		void Render(bool* p_open);
		void Open();
		void Close();
	}
}

#endif
