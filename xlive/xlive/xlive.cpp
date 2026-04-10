#include "stdafx.h"

#include "xlive.h"

#include "H2MOD/GUI/XLiveRendering.h"
#include "H2MOD/GUI/imgui_integration/imgui_handler.h"

#include "Util/Memory.h"
#include "Util/Hooks/Hook.h"

#include "H2MOD/Utils/Utils.h"

#include "H2MOD/GUI/imgui_integration/Console/ImGui_ConsoleImpl.h"

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
	ImGuiHandler::DrawImgui();
	return pXLiveRender();
}

XLIVE_DEFINE_FUNC(XLiveOnResetDevice_t, HRESULT, XLiveOnResetDevice, (VOID* pD3DPP))
{
	XLiveRendering::D3D9ReleaseResources();
	return pXLiveOnResetDevice(pD3DPP);
}

XLIVE_DEFINE_FUNC(XNotifyDelayUI_t, DWORD, XNotifyDelayUI, (ULONG ulMilliSeconds))
{
	return pXNotifyDelayUI(ulMilliSeconds);
}

XLIVE_DEFINE_FUNC(XLivePBufferAllocate_t, HRESULT, XLivePBufferAllocate, (ULONG ulSize, VOID** pxebBuffer))
{
	return pXLivePBufferAllocate(ulSize, pxebBuffer);
}

XLIVE_DEFINE_FUNC(XLivePBufferSetByte_t, HRESULT, XLivePBufferSetByte, (VOID* xebBuffer, ULONG ulOffset, UCHAR ucValue))
{
	return pXLivePBufferSetByte(xebBuffer, ulOffset, ucValue);
}

XLIVE_DEFINE_FUNC(XUserGetXUID_t, DWORD, XUserGetXUID, (DWORD dwUserIndex, XUID* pxuid))
{
	return pXUserGetXUID(dwUserIndex, pxuid);
}

XLIVE_DEFINE_FUNC(XUserGetSigninState_t, XUSER_SIGNIN_STATE, XUserGetSigninState, (DWORD dwUserIndex))
{
	return pXUserGetSigninState(dwUserIndex);
}

XLIVE_DEFINE_FUNC(XShowSigninUI_t, DWORD, XShowSigninUI, (DWORD cPanes, DWORD dwFlags))
{
	return pXShowSigninUI(cPanes, dwFlags);
}

bool XLiveDetoursInitialize()
{
	DETOUR_BEGIN();
	DETOUR_ATTACH(pXLiveInitialize, pXLiveInitialize, XLiveInitialize);
	DETOUR_ATTACH(pXLiveOnResetDevice, pXLiveOnResetDevice, XLiveOnResetDevice);
	DETOUR_ATTACH(pXLiveRender, pXLiveRender, XLiveRender);
	DETOUR_COMMIT();

	return true;
}

bool XLiveSupportedVersion(DWORD dwVersion, DWORD* dwOutSupportedVerIndex)
{
	for (int i = 0; i < ARRAYSIZE(g_XLiveSupportedList); i++)
	{
		const XLIVE_MODULE_VERSION* pVersion = &g_XLiveSupportedList[i];

		if (dwVersion == pVersion->dwVersion)
		{
			*dwOutSupportedVerIndex = (DWORD)i;
			return true;
		}
	}

	return false;
}

bool XLiveModInitialize()
{
	g_hModuleXLive = LoadLibrary(L"xlive.dll");
	assert(g_hModuleXLive != NULL);

	DWORD versionIndex;

	if (!GetModuleFileVersion(g_hModuleXLive, &g_XLiveVersion))
	{
		assert(false);
		return false;
	}

	if (XLiveSupportedVersion(g_XLiveVersion, &versionIndex))
	{
		IMCONSOLE_LOG("XLIVE version loaded: %X - %s", g_XLiveSupportedList[versionIndex].dwVersion, g_XLiveSupportedList[versionIndex].pszVersion);
	}
	else
	{
		IMCONSOLE_LOG("XLIVE version loaded: ( --- unknown version, possible issues --- )");
	}


#define RESOLVE_FUNC_ORD(module, fn, ordinal)				\
{															\
	p##fn = (fn##_t*)GetProcAddress(module, ordinal);		\
	assert(fn##Orig != NULL);								\
}

	RESOLVE_FUNC_ORD(g_hModuleXLive, XLiveInitialize, (const char*)5000);
	RESOLVE_FUNC_ORD(g_hModuleXLive, XLiveOnResetDevice, (const char*)5007);
	RESOLVE_FUNC_ORD(g_hModuleXLive, XLiveRender, (const char*)5002);
	RESOLVE_FUNC_ORD(g_hModuleXLive, XNotifyDelayUI, (const char*)653);

	RESOLVE_FUNC_ORD(g_hModuleXLive, XLivePBufferAllocate, (const char*)5016);
	RESOLVE_FUNC_ORD(g_hModuleXLive, XLivePBufferSetByte, (const char*)5019);

	RESOLVE_FUNC_ORD(g_hModuleXLive, XUserGetXUID, (const char*)5261);
	RESOLVE_FUNC_ORD(g_hModuleXLive, XUserGetSigninState, (const char*)5262);

	RESOLVE_FUNC_ORD(g_hModuleXLive, XShowSigninUI, (const char*)5260)

	XLiveDetoursInitialize();

#undef RESOLVE_FUNC
	return true;
}