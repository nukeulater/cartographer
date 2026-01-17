#pragma once

#include "xlivedefs.h"

#define _WIN32_LEAN_AND_MEAN
#include <windows.h>

HRESULT WINAPI XLiveInitialize_hook(XLIVE_INITIALIZE_INFO* pii);
HRESULT WINAPI XLiveOnResetDevice_hook(VOID* pD3DPP);
DWORD WINAPI XNotifyDelayUI_hook(ULONG ulMilliSeconds);

HRESULT WINAPI XLivePBufferAllocate_hook(
    ULONG ulSize,
    VOID** pxebBuffer
);

HRESULT WINAPI XLivePBufferSetByte_hook(
    VOID* xebBuffer,
    ULONG ulOffset,
    UCHAR ucValue
);

bool GetXLiveModuleTable();

#define XLiveInitialize(pPii) XLiveInitialize_hook(pPii)
#define XLiveOnResetDevice(pD3DPP) XLiveOnResetDevice_hook(pD3DPP)
#define XLivePBufferAllocate(ulSize, pxebBuffer) XLivePBufferAllocate_hook(ulSize, pxebBuffer)
#define XNotifyDelayUI(ulMilliseconds) XNotifyDelayUI_hook(ulMilliseconds)

#define XLivePBufferSetByte(xebBuffer, ulOffset, ucValue) XLivePBufferSetByte_hook(xebBuffer, ulOffset, ucValue)

