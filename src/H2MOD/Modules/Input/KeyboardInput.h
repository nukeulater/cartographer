#pragma once

namespace KeyboardInput
{
	void ToggleKeyboardInput();
	void Initialize();
	void ExecuteHotkey(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}

void hotkeyFuncToggleHideIngameChat();