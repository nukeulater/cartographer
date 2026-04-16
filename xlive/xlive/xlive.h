#pragma once

#include "xlivedefs.h"
#include "xlivedefs_functions.h"

bool XLiveModInitialize();

#define XLIVE_BUILD_MODULE_VERSION(major, minor, build, revision) \
    { XLIVE_BUILD_VERSION(major, minor, build, revision), XLIVE_VERSION_TO_STRING(major, minor, build, revision) }

struct XLIVE_MODULE_VERSION
{
	DWORD dwVersion;
	const char* pszVersion;
};

HRESULT XLiveInitialize(XLIVE_INITIALIZE_INFO* pXii);
HRESULT XLiveOnResetDevice(VOID* pD3DPP);
DWORD XNotifyDelayUI(ULONG ulMilliSeconds);
HRESULT XLivePBufferAllocate(ULONG ulSize, VOID** pxebBuffer);
HRESULT XLivePBufferSetByte(VOID* xebBuffer, ULONG ulOffset, UCHAR ucValue);
DWORD XUserGetXUID(DWORD dwUserIndex, XUID* pxuid);

DWORD XShowSigninUI(DWORD cPanes, DWORD dwFlags);

XUSER_SIGNIN_STATE XUserGetSigninState(DWORD dwUserIndex);
