#include "stdafx.h"
#include "XLiveRendering.h"

#include "imgui.h"

#include "backends/imgui_impl_dx9.h"
#include "H2MOD/GUI/ImGui_Integration/ImGui_Handler.h"
#include "H2MOD/Modules/Input/KeyboardInput.h"

void XLiveRendering::InitializeD3D9(IDirect3DDevice9Ex* pD3DD, D3DPRESENT_PARAMETERS* presentParameters)
{
#ifndef IMGUI_DISABLE
	ImGuiHandler::Initalize(presentParameters->hDeviceWindow);
#endif
}

void XLiveRendering::D3D9ReleaseResources()
{
#ifndef IMGUI_DISABLE
	ImGui_ImplDX9_InvalidateDeviceObjects();
#endif
}

// ### GFWL - FIXME

// #5003: XLiveUninitialize
//int WINAPI XLiveUninitialize()
//{
//	LOG_TRACE_XLIVE("XLiveUninitialize");
//	
//	XLiveRendering::D3D9ReleaseResources();
//	
//	return 0;
//}

