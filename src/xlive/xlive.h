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

HRESULT WINAPI XLiveInitialize(XLIVE_INITIALIZE_INFO* pXii);
HRESULT WINAPI XLiveOnResetDevice(VOID* pD3DPP);
DWORD WINAPI XNotifyDelayUI(ULONG ulMilliSeconds);
HRESULT WINAPI XLivePBufferAllocate(ULONG ulSize, VOID** pxebBuffer);
HRESULT WINAPI XLivePBufferSetByte(VOID* xebBuffer, ULONG ulOffset, UCHAR ucValue);
DWORD WINAPI XUserGetXUID(DWORD dwUserIndex, XUID* pxuid);

DWORD WINAPI XShowSigninUI(DWORD cPanes, DWORD dwFlags);

DWORD WINAPI XCancelOverlapped(PXOVERLAPPED pxOverlapped);

XUSER_SIGNIN_STATE WINAPI XUserGetSigninState(DWORD dwUserIndex);
