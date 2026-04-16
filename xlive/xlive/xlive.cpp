#include "stdafx.h"

#include "xlive.h"

#include "H2MOD/GUI/XLiveRendering.h"
#include "H2MOD/GUI/imgui_integration/imgui_handler.h"
#include "H2MOD/GUI/imgui_integration/Console/ImGui_ConsoleImpl.h"

#include "H2MOD/Utils/Utils.h"

#include "Util/Memory.h"
#include "Util/Hooks/Hook.h"

extern void initialize_instance();

HMODULE g_hModuleXLive = NULL;

DWORD g_XLiveVersion;
char g_XliveVersionStr[128];

XLIVE_MODULE_VERSION g_XLiveSupportedList[] =
{
	XLIVE_BUILD_MODULE_VERSION(1, 2, 241, 0),
	XLIVE_BUILD_MODULE_VERSION(2, 0, 672, 0),
	XLIVE_BUILD_MODULE_VERSION(3, 5, 88, 0),
};

#define XLIVE_DEFINE_FUNC(type, ret, name, args) \
	static type* p##name; \
	ret name args

#define XLIVE_RESOLVE_FUNCTION(module, fn, ordinal, hook)					\
do																			\
{																			\
	p##fn = (fn##_t*)GetProcAddress(module, (const char*)ordinal);			\
	assert(p##fn != NULL);													\
																			\
	if (hook)																\
	{																		\
		DETOUR_ATTACH(p##fn, p##fn, fn);									\
	}																		\
} while (0)

XLIVE_DEFINE_FUNC(XLiveInitialize_t,
	HRESULT, XLiveInitialize, (XLIVE_INITIALIZE_INFO* pXii)
)
{
	initialize_instance();
	if (pXii->pD3D != NULL)
	{
		XLiveRendering::InitializeD3D9(pXii->pD3D, (D3DPRESENT_PARAMETERS*)pXii->pD3DPP);
	}

	return pXLiveInitialize(pXii);
}

XLIVE_DEFINE_FUNC(XLiveRender_t,
	HRESULT, XLiveRender, ()
)
{
#ifndef IMGUI_DISABLE
	ImGuiHandler::DrawImgui();
#endif
	return pXLiveRender();
}

XLIVE_DEFINE_FUNC(XLiveOnResetDevice_t, 
	HRESULT, XLiveOnResetDevice, (VOID* pD3DPP)
)
{
	XLiveRendering::D3D9ReleaseResources();
	return pXLiveOnResetDevice(pD3DPP);
}

XLIVE_DEFINE_FUNC(XNotifyDelayUI_t, 
	DWORD, XNotifyDelayUI, (ULONG ulMilliSeconds)
)
{
	return pXNotifyDelayUI(ulMilliSeconds);
}

XLIVE_DEFINE_FUNC(XLivePBufferAllocate_t, 
	HRESULT, XLivePBufferAllocate, (ULONG ulSize, VOID** pxebBuffer)
)
{
	return pXLivePBufferAllocate(ulSize, pxebBuffer);
}

XLIVE_DEFINE_FUNC(XLivePBufferSetByte_t, 
	HRESULT, XLivePBufferSetByte, (VOID* xebBuffer, ULONG ulOffset, UCHAR ucValue)
)
{
	return pXLivePBufferSetByte(xebBuffer, ulOffset, ucValue);
}

XLIVE_DEFINE_FUNC(XUserGetXUID_t, 
	DWORD, XUserGetXUID, (DWORD dwUserIndex, XUID* pxuid)
)
{
	return pXUserGetXUID(dwUserIndex, pxuid);
}

XLIVE_DEFINE_FUNC(XUserGetSigninState_t, 
	XUSER_SIGNIN_STATE, XUserGetSigninState, (DWORD dwUserIndex)
)
{
	return pXUserGetSigninState(dwUserIndex);
}

XLIVE_DEFINE_FUNC(XShowSigninUI_t, 
	DWORD, XShowSigninUI, (DWORD cPanes, DWORD dwFlags)
)
{
	return pXShowSigninUI(cPanes, dwFlags);
}

bool XLiveGetIsSupportedVersion(DWORD dwVersion, const XLIVE_MODULE_VERSION** dwOutSupportedVerIndex)
{
	for (int i = 0; i < ARRAYSIZE(g_XLiveSupportedList); i++)
	{
		const XLIVE_MODULE_VERSION* pVersion = &g_XLiveSupportedList[i];

		if (dwVersion == pVersion->dwVersion)
		{
			*dwOutSupportedVerIndex = pVersion;
			return true;
		}
	}

	return false;
}

bool XLiveModInitialize()
{
	g_hModuleXLive = LoadLibrary(L"xlive.dll");
	assert(g_hModuleXLive != NULL);

	const XLIVE_MODULE_VERSION* supportedVersion;

	if (!GetModuleFileVersion(g_hModuleXLive, &g_XLiveVersion))
	{
		assert(false);
		return false;
	}

	if (XLiveGetIsSupportedVersion(g_XLiveVersion, &supportedVersion))
	{
		IMCONSOLE_LOG("XLIVE version loaded: %s - %X", supportedVersion->pszVersion, supportedVersion->dwVersion);
	}
	else
	{
		IMCONSOLE_LOG("XLIVE version loaded: ( --- unknown version: %X, possible issues --- )", g_XLiveVersion);
	}

	DETOUR_BEGIN();

	XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XLiveInitialize, 5000, true);
	XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XLiveRender, 5002, true);
	XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XLiveOnResetDevice, 5007, true);
	XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XNotifyDelayUI, 653, false);

	XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XLivePBufferAllocate, 5016, false);
	XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XLivePBufferSetByte, 5019, false);

	XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XUserGetXUID, 5261, false);
	XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XUserGetSigninState, 5262, false);

	XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XShowSigninUI, 5260, false);

	DETOUR_COMMIT();

#undef RESOLVE_FUNC
	return true;
}