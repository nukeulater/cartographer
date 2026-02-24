#include "stdafx.h"

#include "xlive.h"

#include "H2MOD/GUI/XLiveRendering.h"
#include "H2MOD/GUI/imgui_integration/imgui_handler.h"

#include "Util/Memory.h"
#include "Util/Hooks/Hook.h"

extern void initialize_instance();

HMODULE hModuleXLive = NULL;

#define XLIVE_DEFINE_FUNC(type, ret, name, args) \
	static type* name##Orig; \
	static type* name##Hook; \
	ret name args

XLIVE_DEFINE_FUNC(XLiveInitialize_t,
	HRESULT, XLiveInitialize, (XLIVE_INITIALIZE_INFO* pXii)
)
{
	initialize_instance();
	if (pXii->pD3D != NULL)
	{
		XLiveRendering::InitializeD3D9(pXii->pD3D, (D3DPRESENT_PARAMETERS*)pXii->pD3DPP);
	}

	return XLiveInitializeHook(pXii);
}

XLIVE_DEFINE_FUNC(XLiveRender_t,
	HRESULT, XLiveRender, ()
)
{
	ImGuiHandler::DrawImgui();
	return XLiveRenderHook();
}

XLIVE_DEFINE_FUNC(XLiveOnResetDevice_t, HRESULT, XLiveOnResetDevice, (VOID* pD3DPP))
{
	XLiveRendering::D3D9ReleaseResources();
	return XLiveOnResetDeviceHook(pD3DPP);
}

XLIVE_DEFINE_FUNC(XNotifyDelayUI_t, DWORD, XNotifyDelayUI, (ULONG ulMilliSeconds))
{
	return XNotifyDelayUIOrig(ulMilliSeconds);
}

XLIVE_DEFINE_FUNC(XLivePBufferAllocate_t, HRESULT, XLivePBufferAllocate, (ULONG ulSize, VOID** pxebBuffer))
{
	return XLivePBufferAllocateOrig(ulSize, pxebBuffer);
}

XLIVE_DEFINE_FUNC(XLivePBufferSetByte_t, HRESULT, XLivePBufferSetByte, (VOID* xebBuffer, ULONG ulOffset, UCHAR ucValue))
{
	return XLivePBufferSetByteOrig(xebBuffer, ulOffset, ucValue);
}

XLIVE_DEFINE_FUNC(XUserGetXUID_t, DWORD, XUserGetXUID, (DWORD dwUserIndex, XUID* pxuid))
{
	return XUserGetXUIDOrig(dwUserIndex, pxuid);
}

XLIVE_DEFINE_FUNC(XUserGetSigninState_t, XUSER_SIGNIN_STATE, XUserGetSigninState, (DWORD dwUserIndex))
{
	return XUserGetSigninStateOrig(dwUserIndex);
}

XLIVE_DEFINE_FUNC(XShowSigninUI_t, DWORD, XShowSigninUI, (DWORD cPanes, DWORD dwFlags))
{
	return XShowSigninUIOrig(cPanes, dwFlags);
}

bool DetourXLive()
{
	DETOUR_BEGIN();
	DETOUR_ATTACH(XLiveInitializeHook, XLiveInitializeOrig, XLiveInitialize);
	DETOUR_ATTACH(XLiveOnResetDeviceHook, XLiveOnResetDeviceOrig, XLiveOnResetDevice);
	DETOUR_ATTACH(XLiveRenderHook, XLiveRenderOrig, XLiveRender);
	DETOUR_COMMIT();

	return true;
}

bool InitializeXLiveModuleTable()
{
	hModuleXLive = LoadLibrary(L"xlive.dll");
	assert(hModuleXLive != NULL);

#define RESOLVE_FUNC_ORD(module, fn, ordinal)				\
{															\
	fn##Orig = (fn##_t*)GetProcAddress(module, ordinal);	\
	fn##Hook = fn##Orig;									\
	assert(fn##Orig != NULL);								\
}

	RESOLVE_FUNC_ORD(hModuleXLive, XLiveInitialize, (const char*)5000);
	RESOLVE_FUNC_ORD(hModuleXLive, XLiveOnResetDevice, (const char*)5007);
	RESOLVE_FUNC_ORD(hModuleXLive, XLiveRender, (const char*)5002);
	RESOLVE_FUNC_ORD(hModuleXLive, XNotifyDelayUI, (const char*)653);

	RESOLVE_FUNC_ORD(hModuleXLive, XLivePBufferAllocate, (const char*)5016);
	RESOLVE_FUNC_ORD(hModuleXLive, XLivePBufferSetByte, (const char*)5019);

	RESOLVE_FUNC_ORD(hModuleXLive, XUserGetXUID, (const char*)5261);
	RESOLVE_FUNC_ORD(hModuleXLive, XUserGetSigninState, (const char*)5262);

	RESOLVE_FUNC_ORD(hModuleXLive, XShowSigninUI, (const char*)5260)

	DetourXLive();

#undef RESOLVE_FUNC
	return true;
}