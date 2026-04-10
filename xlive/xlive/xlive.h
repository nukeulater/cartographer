#pragma once

#include "xlivedefs.h"

#include "xlive_fn_defs.h"

bool InitializeXLiveModuleTable();

HRESULT XLiveInitialize(XLIVE_INITIALIZE_INFO* pXii);
HRESULT XLiveOnResetDevice(VOID* pD3DPP);
DWORD XNotifyDelayUI(ULONG ulMilliSeconds);
HRESULT XLivePBufferAllocate(ULONG ulSize, VOID** pxebBuffer);
HRESULT XLivePBufferSetByte(VOID* xebBuffer, ULONG ulOffset, UCHAR ucValue);
DWORD XUserGetXUID(DWORD dwUserIndex, XUID* pxuid);

DWORD XShowSigninUI(DWORD cPanes, DWORD dwFlags);

XUSER_SIGNIN_STATE XUserGetSigninState(DWORD dwUserIndex);
