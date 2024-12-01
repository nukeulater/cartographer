#pragma once
#include "input/controllers.h"

/* constants */

extern const char* k_advanced_settings_window_name;
extern const char* k_weapon_offsets_window_name;
extern const char* k_motd_window_name;
extern const char* k_debug_overlay_window_name;

namespace ImGuiHandler
{
	extern bool g_network_stats_overlay;
	extern PDIRECT3DTEXTURE9 g_patch_notes_texture;

	enum s_imgui_images
	{
		patch_notes
	};

	typedef int ImWWindowHandlerFlags;
	enum ImWWindowHandlerFlags_
	{
		_ImWindow_no_input = 1 << 0,
	};

	struct s_imgui_window
	{
		std::string name;
		bool doRender;
		void(__cdecl* renderFunc)(bool*);
		void(__cdecl* openFunc)();
		void(__cdecl* closeFunc)();
		ImWWindowHandlerFlags flags;

		s_imgui_window(const std::string& _name, bool _doRender, void(__cdecl* _renderFunc)(bool*), void(__cdecl* _openFunc)(), void(__cdecl* _closeFunc)(), ImWWindowHandlerFlags _flags = 0)
		{
			name = _name;
			doRender = _doRender;
			renderFunc = _renderFunc;
			openFunc = _openFunc;
			closeFunc = _closeFunc;
			flags = _flags;
		}

		bool NoImInput() const
		{
			return (flags & _ImWindow_no_input) != 0;
		}
	};

	enum e_imgui_aspect_ratio : byte
	{
		four_three,
		sixten_nine
	};

	bool ImGuiShouldHandleInput();
	void ImGuiToggleInput(bool state);
	void SetGameInputState(bool enable);
	bool CanDrawImgui();
	void DrawImgui();
	bool IsWindowActive(const std::string& name);
	void ToggleWindow(const std::string& name);
	void Initalize(LPDIRECT3DDEVICE9 pDevice, HWND hWnd);
	float WidthPercentage(float percent);
	void TextVerticalPad(const char* label);
	bool LoadTextureFromFile(const wchar_t* filename, s_imgui_images image, int* out_width, int* out_height);
	PDIRECT3DTEXTURE9 GetTexture(s_imgui_images image);
	void ReleaseTextures();
	e_imgui_aspect_ratio GetAspectRatio(const real_point2d* display_size);
	namespace ImMOTD {
		bool DownloadMOTD(const std::wstring& motd_path, e_imgui_aspect_ratio ratio);
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
	namespace ImDebugOverlay
	{
		void Render(bool* p_open);
		void AddWatchItem(std::string Key, std::string Description);
		void UpdateWatchItem(std::string Key, std::string Value);
		void Open();
		void Close();
	}
	namespace ImMessageBox
	{
		void Render(bool* p_open);
		void SetMessage(std::string message);
		void Open();
		void Close();

		extern std::string windowName;
	}
	namespace WeaponOffsets
	{
		void Render(bool* p_open);
		void Open();
		void Close();
		void MapLoad();
		void Initialize();
	}
}