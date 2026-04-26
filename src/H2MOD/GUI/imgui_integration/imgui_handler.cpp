#include "stdafx.h"
#include "imgui_handler.h"

#ifndef IMGUI_DISABLE

#include "game/player_control.h"
#include "interface/user_interface_guide.h"
#include "rasterizer/dx9/rasterizer_dx9_main.h"

#include "imgui.h"
#include "backends/imgui_impl_dx9.h"
#include "backends/imgui_impl_win32.h"

#include "ImGui_CartographerStyle.h"
#include "Console/ImGui_ConsoleImpl.h"
#include "ImGui_NetworkStatsOverlay.h"

#include "imgui_ProdigyCleanTTF.h"

ImFont*											g_im_font_ProdigyClean = NULL;
c_static_flags_no_init<k_imgui_window_count>	g_imgui_window_should_render = {};

struct s_imgui_window
{
	const char* name;
	void(* const pDraw)(bool*);
	void(* const pOpen)(void);
	void(* const pClose)(void);
	const uint32 flags;
};

enum e_im_window_handler_flags
{
	_im_window_no_input_bit = 0,
	_im_window_flag_count = 32
};

namespace ImGuiHandler
{
	const s_imgui_window k_weapon_offsets_window =
		{ "Weapon Offsets", WeaponOffsets::Render, WeaponOffsets::Open, WeaponOffsets::Close, 0 };
	const s_imgui_window k_message_box_window =
		{ "Message Box", ImMessageBox::Render, ImMessageBox::Open, ImMessageBox::Close, 0 };
	const s_imgui_window k_advanced_settings_window =
		{ "Advanced Settings", ImAdvancedSettings::Render, ImAdvancedSettings::Open, ImAdvancedSettings::Close, 0};
#ifdef TERMINAL_ENABLED
	const s_imgui_window k_cartographer_console_window =
		{ "Cartographer Console", CartographerConsole::Render, CartographerConsole::Open, CartographerConsole::Close, 0};
#else
#define k_cartographer_console_window
#endif

	const s_imgui_window k_imgui_windows[k_imgui_window_count] =
	{
		k_weapon_offsets_window, k_message_box_window, k_advanced_settings_window, k_cartographer_console_window
	};

	namespace 
	{
		bool				clear_imgui_input_state = true;
	}

	int						g_network_stats_overlay = _network_stats_display_none;

	void Initalize(HWND hWnd)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplWin32_Init((void*)hWnd);

		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		imgui::Cartographer::StyleSetAsDefault();

		ImFontConfig fontConfig;
		fontConfig.SizePixels = 13.0f * 1.5f;
		g_im_font_ProdigyClean = io.Fonts->AddFontFromMemoryCompressedBase85TTF(
			proggy_clean_ttf_compressed_data_base85,
			fontConfig.SizePixels,
			&fontConfig);

		ImGui_ImplDX9_Init(rasterizer_dx9_device_get_interface());

		atexit([]() {
			ImGui_ImplDX9_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
		});
	}

	bool ShouldCaptureInput()
	{
		bool result = false;

		for (int32 i = 0; i < k_imgui_window_count; i++)
		{
			const s_imgui_window* const imWindow = &k_imgui_windows[i];
			if (g_imgui_window_should_render.test(i) && !TEST_BIT(imWindow->flags, _im_window_no_input_bit))
			{
				result = true;
				break;
			}
		}

		return result;
	}

	void DrawUpdate()
	{
		// clear keyboard/mouse input state if we are about to close the ImGui windows
		if (clear_imgui_input_state)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.ClearInputKeys();
			io.ClearInputMouse();
			clear_imgui_input_state = false;
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::PushFont(g_im_font_ProdigyClean);

		static bool display_network_stats = false;
		display_network_stats = g_network_stats_overlay == _network_stats_display_complete;
		ShowNetworkStatsOverlay(&display_network_stats);
		for (int32 i = 0; i < k_imgui_window_count; ++i)
		{
			bool should_render = g_imgui_window_should_render.test(i);
			if (should_render)
			{
				const s_imgui_window* const window = &k_imgui_windows[i];
				window->pDraw(&should_render);
			}
		}

		ImGui::PopFont();

		// Rendering
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	void WindowToggle(e_imgui_window window)
	{
		ASSERT(IN_RANGE(window, _imgui_window_weapon_offsets, k_imgui_window_count));

		const s_imgui_window* const imWindow = &k_imgui_windows[window];

		// Toggle render
		g_imgui_window_should_render.set(window, !g_imgui_window_should_render.test(window));
		if (g_imgui_window_should_render.test(window))
		{
			imWindow->pOpen();
		}
		else
		{
			imWindow->pClose();
		}

		if (!TEST_BIT(imWindow->flags, _im_window_no_input_bit))
		{
			user_interface_guide_state_manager_get()->set_input_captured_by_shell(g_imgui_window_should_render.test(window));
		}

		// check if the window blocks the input of the game
		bool block_game_input = ShouldCaptureInput();
		clear_imgui_input_state = !block_game_input;
	}

	bool WindowIsActive(e_imgui_window window)
	{
		return g_imgui_window_should_render.test(window);
	}

	float WidthPercentage(float percent)
	{
		auto width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		if (ImGui::GetColumnsCount() > 1)
			width = ImGui::GetColumnWidth();

		return width * (percent / 100.0f);
	}

	void TextVerticalPad(const char* label)
	{
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label);
	}
}

#endif
