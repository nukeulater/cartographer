#include "stdafx.h"

#include "xlive.h"

#include "H2MOD/GUI/XLiveRendering.h"
#include "H2MOD/GUI/imgui_integration/imgui_handler.h"

HMODULE hModuleXLive = NULL;

#define XLIVE_FUNC_DEFINE(def, func, args) \
	static def (WINAPI *func##Orig) args; \
	def WINAPI func##_hook args

XLIVE_FUNC_DEFINE(HRESULT, XLiveInitialize, (XLIVE_INITIALIZE_INFO* pii))
{
	//XLiveRendering::InitializeD3D9((D3DPRESENT_PARAMETERS*)pii->pD3DPP);
	return XLiveInitializeOrig(pii);
}

XLIVE_FUNC_DEFINE(HRESULT, XLiveRender, ())
{
	//ImGuiHandler::DrawImgui();
	return XLiveRenderOrig();
}

XLIVE_FUNC_DEFINE(HRESULT, XLiveOnDestroyDevice, ())
{
	//XLiveRendering::D3D9ReleaseResources();
	return XLiveOnDestroyDeviceOrig();
}

XLIVE_FUNC_DEFINE(HRESULT, XLiveOnResetDevice, (VOID* pD3DPP))
{
	//XLiveRendering::D3D9ReleaseResources();
	return XLiveOnResetDeviceOrig(pD3DPP);
}

XLIVE_FUNC_DEFINE(DWORD, XNotifyDelayUI, (ULONG ulMilliSeconds))
{
	return XNotifyDelayUIOrig(ulMilliSeconds);
}

XLIVE_FUNC_DEFINE(HRESULT, XLivePBufferAllocate, (
	ULONG ulSize,
	VOID** pxebBuffer))
{
	return XLivePBufferAllocateOrig(ulSize, pxebBuffer);
}

XLIVE_FUNC_DEFINE(HRESULT, XLivePBufferSetByte, (
	VOID* xebBuffer,
	ULONG ulOffset,
	UCHAR ucValue))
{
	return XLivePBufferSetByteOrig(xebBuffer, ulOffset, ucValue);
}

XLIVE_FUNC_DEFINE(DWORD, XUserGetXUID, (
	DWORD dwUserIndex,
	XUID* pxuid
	))
{
	return XUserGetXUIDOrig(dwUserIndex, pxuid);
}

bool GetXLiveModuleTable()
{
	hModuleXLive = LoadLibrary(L"xlive.dll");
	assert(hModuleXLive != NULL);

#define RESOLVE_FUNC_ORD(module, fn, ordinal) \
do { \
	fn##Orig = (decltype(fn##_hook)*)GetProcAddress(module, ordinal); \
	assert(fn##Orig != NULL); \
} while (0)

	RESOLVE_FUNC_ORD(hModuleXLive, XLiveInitialize, (const char*)5000);
	RESOLVE_FUNC_ORD(hModuleXLive, XLiveOnResetDevice, (const char*)5007);
	RESOLVE_FUNC_ORD(hModuleXLive, XLiveRender, (const char*)5002);
	RESOLVE_FUNC_ORD(hModuleXLive, XLiveOnDestroyDevice, (const char*)5006);
	RESOLVE_FUNC_ORD(hModuleXLive, XNotifyDelayUI, (const char*)653);

	RESOLVE_FUNC_ORD(hModuleXLive, XLivePBufferAllocate, (const char*)5016);
	RESOLVE_FUNC_ORD(hModuleXLive, XLivePBufferSetByte, (const char*)5019);

	RESOLVE_FUNC_ORD(hModuleXLive, XUserGetXUID, (const char*)5261);

#undef RESOLVE_FUNC
	return true;
}