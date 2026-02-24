#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>

#include "xlivedefs.h"

// Socket Functions
typedef int (WSAAPI XWSAStartup_t)(WORD wVersionRequested, LPWSADATA lpWSAData);
typedef int (WSAAPI XWSACleanup_t)(void);
typedef SOCKET(WSAAPI XSocketCreate_t)(int af, int type, int protocol);
typedef int (WSAAPI XSocketClose_t)(SOCKET s);
typedef int (WSAAPI XSocketShutdown_t)(SOCKET s, int how);
typedef int (WSAAPI XSocketIOCTLSocket_t)(SOCKET s, long cmd, u_long* argp);
typedef int (WSAAPI XSocketSetSockOpt_t)(SOCKET s, int level, int optname, const char* optval, int optlen);
typedef int (WSAAPI XSocketGetSockOpt_t)(SOCKET s, int level, int optname, char* optval, int* optlen);
typedef int (WSAAPI XSocketBind_t)(SOCKET s, const struct sockaddr* name, int namelen);
typedef int (WSAAPI XSocketConnect_t)(SOCKET s, const struct sockaddr* name, int namelen);
typedef int (WSAAPI XSocketListen_t)(SOCKET s, int backlog);
typedef SOCKET(WSAAPI XSocketAccept_t)(SOCKET s, struct sockaddr* addr, int* addrlen);
typedef int (WSAAPI XSocketSelect_t)(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const struct timeval* timeout);
typedef int (WSAAPI XSocketRecv_t)(SOCKET s, char* buf, int len, int flags);
typedef int (WSAAPI XSocketRecvFrom_t)(SOCKET s, char* buf, int len, int flags, struct sockaddr* from, int* fromlen);
typedef int (WSAAPI XSocketSend_t)(SOCKET s, const char* buf, int len, int flags);
typedef int (WSAAPI XSocketSendTo_t)(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen);
typedef int (WSAAPI XSocketWSAGetLastError_t)(void);
typedef u_long(WSAAPI XSocketHTONL_t)(u_long hostlong);
typedef u_short(WSAAPI XSocketHTONS_t)(u_short hostshort);
typedef u_long(WSAAPI XSocketNTOHL_t)(u_long netlong);
typedef u_short(WSAAPI XSocketNTOHS_t)(u_short netshort);

// XNet Functions
typedef INT(WSAAPI XNetStartup_t)(const XNetStartupParams* pxnsp);
typedef INT(WSAAPI XNetCleanup_t)(void);
typedef INT(WSAAPI XNetRandom_t)(BYTE* pb, UINT cb);
typedef INT(WSAAPI XNetCreateKey_t)(XNKID* pxnkid, XNKEY* pxnkey);
typedef INT(WSAAPI XNetRegisterKey_t)(const XNKID* pxnkid, const XNKEY* pxnkey);
typedef INT(WSAAPI XNetUnregisterKey_t)(const XNKID* pxnkid);
typedef INT(WSAAPI XNetXnAddrToInAddr_t)(const XNADDR* pxna, const XNKID* pxnkid, IN_ADDR* pina);
typedef INT(WSAAPI XNetInAddrToXnAddr_t)(const IN_ADDR ina, XNADDR* pxna, XNKID* pxnkid);
typedef INT(WSAAPI XNetUnregisterInAddr_t)(const IN_ADDR ina);
typedef INT(WSAAPI XNetConnect_t)(const IN_ADDR ina);
typedef DWORD(WSAAPI XNetGetConnectStatus_t)(const IN_ADDR ina);
typedef INT(WSAAPI XNetQosListen_t)(const XNKID* pxnkid, const BYTE* pb, UINT cb, DWORD dwBitsPerSec, DWORD dwFlags);
typedef INT(WSAAPI XNetQosLookup_t)(UINT cxna, const XNADDR* apxna[], const XNKID* apxnkid[], const XNKEY* apxnkey[], UINT cina, const IN_ADDR aina[], const DWORD adwServiceId[], UINT cProbes, DWORD dwBitsPerSec, DWORD dwFlags, WSAEVENT hEvent, XNQOS** ppxnqos);
typedef INT(WSAAPI XNetQosRelease_t)(XNQOS* pxnqos);
typedef DWORD(WSAAPI XNetGetTitleXnAddr_t)(XNADDR* pxna);
typedef DWORD(WSAAPI XNetGetEthernetLinkStatus_t)(void);
typedef INT(WSAAPI XNetSetSystemLinkPort_t)(WORD wSystemLinkPort);

// XCustom Functions
typedef VOID(WINAPI XCustomSetAction_t)(DWORD dwActionIndex, LPCWSTR szActionText, DWORD dwFlags);
typedef BOOL(WINAPI XCustomGetLastActionPress_t)(DWORD* pdwUserIndex, DWORD* pdwActionIndex, XUID* pXuid);

// XNotify Functions
typedef BOOL(WINAPI XNotifyGetNext_t)(HANDLE hNotification, DWORD dwMsgFilter, PDWORD pdwId, PULONG_PTR pParam);
typedef DWORD(WINAPI XNotifyDelayUI_t)(ULONG ulMilliSeconds);

// XGet Overlapped Functions
typedef DWORD(WINAPI XGetOverlappedExtendedError_t)(PXOVERLAPPED pOverlapped);
typedef DWORD(WINAPI XGetOverlappedResult_t)(PXOVERLAPPED pOverlapped, LPDWORD pdwResult, BOOL bWait);

// XLive Core Functions
typedef HRESULT(WINAPI XLiveInitialize_t)(XLIVE_INITIALIZE_INFO* pXii);  // Approximate based on XLiveInitializeEx
typedef HRESULT(WINAPI XLiveInput_t)(XLIVE_INPUT_INFO* pXii);
typedef HRESULT(WINAPI XLiveRender_t)(void);
typedef void (WINAPI XLiveUninitialize_t)(void);
typedef HRESULT(WINAPI XLiveOnResetDevice_t)(VOID* pD3DPP);
typedef HRESULT(WINAPI XLivePBufferAllocate_t)(ULONG ulSize, XLIVE_PROTECTED_BUFFER_PTR pxebBuffer);
typedef HRESULT(WINAPI XLivePBufferGetByte_t)(XLIVE_PROTECTED_BUFFER xebBuffer, ULONG ulOffset, UCHAR* pucValue);
typedef HRESULT(WINAPI XLivePBufferSetByte_t)(XLIVE_PROTECTED_BUFFER xebBuffer, ULONG ulOffset, UCHAR ucValue);
typedef HRESULT(WINAPI XLiveGetUpdateInformation_t)(void* pXLiveUpdateInfo);
typedef HRESULT(WINAPI XLiveUpdateSystem_t)(LPCWSTR lpszRelaunchCmdLine);
typedef HMODULE(WINAPI XLiveLoadLibraryEx_t)(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
typedef HRESULT(WINAPI XLiveFreeLibrary_t)(HMODULE hModule);
typedef BOOL(WINAPI XLivePreTranslateMessage_t)(const LPMSG lpMsg);

// XShow UI Functions
typedef DWORD(WINAPI XShowCustomPlayerListUI_t)(DWORD dwUserIndex, DWORD dwFlags, LPCWSTR pszTitle, LPCWSTR pszDescription, CONST BYTE* pbImage, DWORD cbImage, CONST XPLAYERLIST_USER* rgPlayers, DWORD cPlayers, CONST XPLAYERLIST_BUTTON* pXButton, CONST XPLAYERLIST_BUTTON* pYButton, XPLAYERLIST_RESULT* pResults, XOVERLAPPED* pOverlapped);
typedef DWORD(WINAPI XShowGuideUI_t)(DWORD dwUserIndex);
typedef DWORD(WINAPI XShowGamerCardUI_t)(DWORD dwUserIndex, XUID XuidPlayer);
typedef DWORD(WINAPI XShowSigninUI_t)(DWORD cPanes, DWORD dwFlags);

// XLocator Functions
typedef HRESULT(WINAPI XLocatorServerAdvertise_t)(DWORD dwUserIndex, DWORD dwServerType, XNKID xnkid, XNKEY xnkey, DWORD dwMaxPublicSlots, DWORD dwMaxPrivateSlots, DWORD dwFilledPublicSlots, DWORD dwFilledPrivateSlots, DWORD cProperties, PXUSER_PROPERTY pProperties, PXOVERLAPPED pXOverlapped);
typedef HRESULT(WINAPI XLocatorServerUnAdvertise_t)(DWORD dwUserIndex, PXOVERLAPPED pXOverlapped);
typedef HRESULT(WINAPI XLocatorGetServiceProperty_t)(DWORD dwUserIndex, DWORD cNumProperties, PXUSER_PROPERTY pProperties, PXOVERLAPPED pXOverlapped);
typedef DWORD(WINAPI XLocatorCreateServerEnumerator_t)(DWORD dwUserIndex, DWORD cItems, DWORD cRequiredPropertyIDs, DWORD* pRequiredPropertyIDs, DWORD cFilterGroupItems, XLOCATOR_FILTER_GROUP* pxlFilterGroups, DWORD cSorterItems, XLOCATOR_SORTER* pxlSorters, DWORD* pcbBuffer, PHANDLE phEnum);
typedef HRESULT(WINAPI XLocatorServiceInitialize_t)(void* pXii, PHANDLE phLocatorService);
typedef HRESULT(WINAPI XLocatorServiceUnInitialize_t)(HANDLE hLocatorService);
typedef HRESULT(WINAPI XLocatorCreateKey_t)(XNKID* pxnkid, XNKEY* pxnkey);

// Handle Functions
typedef BOOL(WINAPI XCloseHandle_t)(HANDLE hObject);

// Overlapped Functions
typedef DWORD(WINAPI XCancelOverlapped_t)(PXOVERLAPPED pOverlapped);

// Enumerate Functions
typedef DWORD(WINAPI XEnumerate_t)(HANDLE hEnum, PVOID pvBuffer, DWORD cbBuffer, PDWORD pcItemsReturned, PXOVERLAPPED pOverlapped);

// XUser Functions
typedef DWORD(WINAPI XUserGetXUID_t)(DWORD dwUserIndex, XUID*);
typedef XUSER_SIGNIN_STATE(WINAPI XUserGetSigninState_t)(DWORD dwUserIndex);
typedef DWORD(WINAPI XUserGetName_t)(DWORD dwUserIndex, LPSTR szUserName, DWORD cchUserName);
typedef DWORD(WINAPI XUserCheckPrivilege_t)(DWORD dwUserIndex, XPRIVILEGE_TYPE PrivilegeType, PBOOL pfResult);
typedef VOID(WINAPI XUserSetProperty_t)(DWORD dwUserIndex, DWORD dwPropertyId, DWORD cbValue, CONST VOID* pvValue);
typedef VOID(WINAPI XUserSetContext_t)(DWORD dwUserIndex, DWORD dwContextId, DWORD dwContextValue);
typedef DWORD(WINAPI XUserWriteAchievements_t)(DWORD dwNumAchievements, CONST XUSER_ACHIEVEMENT* pAchievements, PXOVERLAPPED pOverlapped);
typedef DWORD(WINAPI XUserCreateAchievementEnumerator_t)(DWORD dwTitleId, DWORD dwUserIndex, XUID xuid, DWORD dwDetailFlags, DWORD dwStartingIndex, DWORD cItem, PDWORD pcbBuffer, PHANDLE ph);
typedef DWORD(WINAPI XUserMuteListQuery_t)(DWORD dwUserIndex, XUID XuidRemoteTalker, BOOL* pfOnMuteList);
typedef DWORD(WINAPI XUserReadProfileSettings_t)(DWORD dwTitleId, DWORD dwUserIndex, DWORD dwNumSettingIds, const DWORD* pdwSettingIds, DWORD* pcbResults, PXUSER_READ_PROFILE_SETTING_RESULT pResults, PXOVERLAPPED pXOverlapped);

// Notify Functions
typedef HANDLE(WINAPI XNotifyCreateListener_t)(ULONGLONG qwAreas);

// Session Functions
typedef DWORD(WINAPI XSessionCreate_t)(DWORD dwFlags, DWORD dwUserIndex, DWORD dwMaxPublicSlots, DWORD dwMaxPrivateSlots, ULONGLONG* pqwSessionNonce, PXSESSION_INFO pSessionInfo, PXOVERLAPPED pXOverlapped, HANDLE* ph);
typedef DWORD(WINAPI XSessionStart_t)(HANDLE hSession, DWORD dwFlags, PXOVERLAPPED pXOverlapped);
typedef DWORD(WINAPI XSessionLeaveLocal_t)(HANDLE hSession, DWORD dwUserCount, const DWORD* pdwUserIndexes, PXOVERLAPPED pXOverlapped);
typedef DWORD(WINAPI XSessionJoinRemote_t)(HANDLE hSession, DWORD dwXuidCount, const XUID* pXuids, const BOOL* pfPrivateSlots, PXOVERLAPPED pXOverlapped);
typedef DWORD(WINAPI XSessionJoinLocal_t)(HANDLE hSession, DWORD dwUserCount, const DWORD* pdwUserIndexes, const BOOL* pfPrivateSlots, PXOVERLAPPED pXOverlapped);
typedef DWORD(WINAPI XSessionDelete_t)(HANDLE hSession, PXOVERLAPPED pXOverlapped);
typedef DWORD(WINAPI XSessionEnd_t)(HANDLE hSession, PXOVERLAPPED pXOverlapped);
typedef DWORD(WINAPI XSessionLeaveRemote_t)(HANDLE hSession, DWORD dwXuidCount, const XUID* pXuids, PXOVERLAPPED pXOverlapped);

// String Functions
typedef DWORD(WINAPI XStringVerify_t)(DWORD dwFlags, const CHAR* szLocale, DWORD dwNumStrings, const STRING_DATA* pStringData, DWORD cbResults, STRING_VERIFY_RESPONSE* pResults, XOVERLAPPED* pXOverlapped);

// Storage Functions
typedef DWORD(WINAPI XStorageUploadFromMemory_t)(DWORD dwUserIndex, const WCHAR* wszServerPath, DWORD dwBufferSize, const BYTE* pbBuffer, XOVERLAPPED* pXOverlapped);
typedef DWORD(WINAPI XStorageDelete_t)(DWORD dwUserIndex, const WCHAR* wszServerPath, XOVERLAPPED* pXOverlapped);
typedef DWORD(WINAPI XStorageBuildServerPath_t)(DWORD dwUserIndex, int StorageFacility, CONST void* pvStorageFacilityInfo, DWORD dwStorageFacilityInfoSize, LPCWSTR pwszItemName, WCHAR* pwszServerPath, DWORD* pdwServerPathLength);
typedef DWORD(WINAPI XStorageDownloadToMemory_t)(DWORD dwUserIndex, const WCHAR* wszServerPath, DWORD dwBufferSize, const BYTE* pbBuffer, DWORD cbResults, void* pResults, XOVERLAPPED* pXOverlapped);

// Online Functions
typedef DWORD(WINAPI XOnlineStartup_t)(void);

// Friends Functions
typedef DWORD(WINAPI XFriendsCreateEnumerator_t)(DWORD dwUserIndex, DWORD dwStartingIndex, DWORD dwFriendsToReturn, DWORD* pcbBuffer, HANDLE* ph);

// Invite Functions
typedef DWORD(WINAPI XInviteGetAcceptedInfo_t)(DWORD dwUserIndex, XINVITE_INFO* pInfo);

// HV Functions
typedef HRESULT(WINAPI XHVCreateEngine_t)(void* pParams, PHANDLE phWorkerThread, void** ppEngine);