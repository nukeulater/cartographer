#include "stdafx.h"

#include "xlive.h"
#include "xlivedefs_ordinals.h"

#include "H2MOD/GUI/XLiveRendering.h"
#include "H2MOD/GUI/imgui_integration/imgui_handler.h"
#include "H2MOD/GUI/imgui_integration/Console/ImGui_ConsoleImpl.h"

#include "H2MOD/Utils/Utils.h"

#include "Util/Memory.h"
#include "Util/Hooks/Hook.h"

#include "interface/user_interface_guide.h"

extern void initialize_instance();

HMODULE g_hModuleXLive = NULL;

DWORD g_XLiveVersion;
char g_XliveVersionStr[128];

const XLIVE_MODULE_VERSION g_XLiveSupportedList[] =
{
	XLIVE_BUILD_MODULE_VERSION(1, 2, 241, 0),
	XLIVE_BUILD_MODULE_VERSION(2, 0, 672, 0),
	XLIVE_BUILD_MODULE_VERSION(3, 5, 88, 0),
};

#define XLIVE_DEFINE_FUNC(type, ret, name, args) \
	static type* p##name; \
	ret WINAPI name args

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
	ImGuiHandler::DrawUpdate();
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

XLIVE_DEFINE_FUNC(XNotifyGetNext_t,
	BOOL, XNotifyGetNext, (HANDLE hNotification, DWORD dwMsgFilter, PDWORD pdwId, PULONG_PTR pParam)
)
{
	BOOL result = pXNotifyGetNext(hNotification, dwMsgFilter, pdwId, pParam);
	if (*pdwId == 0x9)
	{
		c_user_interface_guide_state_manager* guide_state_manager = user_interface_guide_state_manager_get();
		guide_state_manager->set_xlive_capturing_input(!!(*(ULONG*)pParam));
	}

	return result;
}

XLIVE_DEFINE_FUNC(XCancelOverlapped_t,
	DWORD, XCancelOverlapped, (PXOVERLAPPED pOverlapped)
)
{
	return pXCancelOverlapped(pOverlapped);
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
	{
		XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XLiveInitialize,			XLIVE_ORDINAL_XLIVEINITIALIZE, true);
		XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XLiveRender,				XLIVE_ORDINAL_XLIVERENDER, true);
		XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XLiveOnResetDevice,		XLIVE_ORDINAL_XLIVEONRESETDEVICE, true);
		XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XNotifyDelayUI,			XLIVE_ORDINAL_XNOTIFYDELAYUI, false);

		XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XLivePBufferAllocate,	XLIVE_ORDINAL_XLIVEPBUFFERALLOCATE, false);
		XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XLivePBufferSetByte,		XLIVE_ORDINAL_XLIVEPBUFFERSETBYTE, false);

		XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XUserGetXUID,			XLIVE_ORDINAL_XUSERGETXUID, false);
		XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XUserGetSigninState,		XLIVE_ORDINAL_XUSERGETSIGNINSTATE, false);

		XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XShowSigninUI,			XLIVE_ORDINAL_XSHOWSIGNINUI, false);

		XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XNotifyGetNext,			XLIVE_ORDINAL_XNOTIFYGETNEXT, true);

		XLIVE_RESOLVE_FUNCTION(g_hModuleXLive, XNotifyGetNext,			XLIVE_ORDINAL_XCANCELOVERLAPPED, false);
	}
	DETOUR_COMMIT();

	return true;
}