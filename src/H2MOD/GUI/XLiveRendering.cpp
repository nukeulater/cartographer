#include "stdafx.h"
#include "XLiveRendering.h"

#include "imgui.h"

#include "backends/imgui_impl_dx9.h"
#include "H2MOD/GUI/ImGui_Integration/ImGui_Handler.h"
#include "H2MOD/Modules/Input/KeyboardInput.h"

static D3DPRESENT_PARAMETERS g_d3dPresentParameters;
static IDirect3DDevice9Ex* g_xlive_d3d_device;
static CRITICAL_SECTION g_render_section;

void XLiveRendering::InitializeD3D9(IDirect3DDevice9Ex* pD3DD, D3DPRESENT_PARAMETERS* presentParameters)
{
	InitializeCriticalSection(&g_render_section);

	g_d3dPresentParameters = *presentParameters;

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
//	DeleteCriticalSection(&g_render_section);
//	return 0;
//}

// ### GFWL - FIXME

//// #5007: XLiveOnResetDevice
//int WINAPI XLiveOnResetDevice(D3DPRESENT_PARAMETERS* pD3DPP)
//{
//	g_d3dPresentParameters = *pD3DPP;
//
//	//Have to invalidate ImGUI on device reset, otherwise it hangs the device in a reset loop.
//	//https://github.com/ocornut/imgui/issues/1464#issuecomment-347469716
//
//	XLiveRendering::D3D9ReleaseResources();
//
//	//LOG_TRACE_XLIVE("XLiveOnResetDevice");
//	return 0;
//}

