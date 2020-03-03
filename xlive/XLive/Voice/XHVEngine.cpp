#include "stdafx.h"
#include "XHVEngine.h"

#include "Globals.h"

#include "DSound.h"
#include "3rdparty\alt-voice\include\alt-voice.h"

#include "H2MOD\Modules\Config\Config.h"

#include "3rdparty/alt-voice/src/CRingBuffer.h"

ISoundInput* soundInput;
ISoundOutput* soundOutput;

IStreamPlayer* streamPlayer1 = nullptr;

bool isTalking = false;

struct VoicePacket { char data[10]; };
RingBuffer<char, 10000> packetBuffer;

void on_voice_input_callback(const void* buffer, int length, short micLevel)
{
	isTalking = false;
	if (micLevel > 500)
	{
		if (length <= XHV_VOICECHAT_MODE_PACKET_SIZE * XHV_MAX_VOICECHAT_PACKETS)
		{
			isTalking = true;
			packetBuffer.Write((char*)&length, sizeof(int));
			packetBuffer.Write((char*)buffer, length);

			/*if (streamPlayer1 != nullptr)
			{
				streamPlayer1->PushOpusBuffer(buffer, length);
				streamPlayer1->Update();
			}*/
		}
	}
}

//#5008
int WINAPI XHVCreateEngine(PXHV_INIT_PARAMS pParams, PHANDLE phWorkerThread, PIXHV2ENGINE *ppEngine)
{
#if COMPILE_WITH_VOICE
	if (H2Config_voice_chat)
	{
		char *inputDeviceEnum = GetInputDevicesEnum();
		char *outputDeviceEnum = GetOutputDevicesEnum();

		char *inputDeviceName = GetNextDevice(&inputDeviceEnum);
		char *outputDeviceName = GetNextDevice(&outputDeviceEnum);

		int bitrate = 16000;
		int frequency = 24000;
		int frames_per_buffer = 240;

		if (inputDeviceName && outputDeviceName) {
			LOG_TRACE(voice_log, "XHVCreateEngine() - input device name: {}, output: {}", inputDeviceName, outputDeviceName);
			AltVoiceError err = CreateSoundInput(inputDeviceName, frequency, frames_per_buffer, bitrate, &soundInput);
			if (err)
			{
				LOG_TRACE(voice_log, "XHVCreateEngine() - failed to create input device: {}", inputDeviceName);
				return DSERR_NODRIVER;
			}
			soundInput->RegisterCallback(on_voice_input_callback);

			err = CreateSoundOutput(outputDeviceName, frequency, 32, &soundOutput);
			if (err)
			{
				LOG_TRACE(voice_log, "XHVCreateEngine() - failed to create output device: {}", outputDeviceName);
				return DSERR_NODRIVER;
			}
		}
		else
			return DSERR_NODRIVER;

		if (pParams->bCustomVADProvided)
		{
			LOG_TRACE(voice_log, "XHVCreateEngine() - bCustomVADProvided set");
		}

		if (ppEngine)
		{
			*ppEngine = new XHVENGINE();
			LOG_TRACE(voice_log, "XHVCreateEngine() - created engine");

			return S_OK;
		}
	}
#endif

	return DSERR_NODRIVER;
}

BOOL XHVENGINE::IsHeadsetPresent(VOID *pthis, DWORD dwUserIndex) {
	//LOG_TRACE("IsHeadsetPresent()");

#if COMPILE_WITH_VOICE
	return soundInput != nullptr && soundOutput != nullptr;
#endif
	return false;
}

BOOL XHVENGINE::isRemoteTalking(VOID *pthis, XUID xuid) {
	//LOG_TRACE(voice_log, "IXHV2Engine::isRemoteTalking()");
#if COMPILE_WITH_VOICE
	return soundOutput->GetStreamPlayer(xuid)->IsPlaying();
#endif
	return false;
}

BOOL XHVENGINE::IsLocalTalking(VOID *pthis, DWORD dwUserIndex) {
	//LOG_TRACE(voice_log, "IXHV2Engine::isTalking());
	//check the xuid map
#if COMPILE_WITH_VOICE
	return isTalking;
#endif
	return false;
}

LONG XHVENGINE::AddRef(VOID *pthis)
{
	LOG_TRACE(voice_log, "IXHV2Engine::AddRef");
	return S_OK;
}

LONG XHVENGINE::Release(VOID *pthis)
{
	LOG_TRACE(voice_log, "IXHV2Engine::Release");

#if COMPILE_WITH_VOICE
	DestroySoundInput(soundInput);
	DestroySoundOutput(soundOutput);
#endif

	return S_OK;
}


HRESULT XHVENGINE::Lock(VOID *pthis, XHV_LOCK_TYPE lockType)
{
	if (lockType == XHV_LOCK_TYPE_LOCK)
	{
		soundInput->Lock();
		return S_OK;
	}

	if (lockType == XHV_LOCK_TYPE_TRYLOCK)
	{
		return soundInput->TryLock() == true ? S_OK : E_FAIL;
	}

	if (lockType == XHV_LOCK_TYPE_UNLOCK)
	{
		soundInput->Unlock();
		return S_OK;
	}

	return S_OK;
}

HRESULT XHVENGINE::StartLocalProcessingModes(VOID *pthis, DWORD dwUserIndex, /* CONST PXHV_PROCESSING_MODE*/ VOID *processingModes, DWORD dwNumProcessingModes)
{
	if (H2GetInstanceId() == 1)
		soundInput->EnableInput();
	return S_OK;
}

HRESULT XHVENGINE::StopLocalProcessingModes(VOID *pthis, DWORD dwUserIndex, /*CONST PXHV_PROCESSING_MODE*/ VOID *processingModes, DWORD dwNumProcessingModes)
{
	soundInput->DisableInput();
	return S_OK;
}

HRESULT XHVENGINE::StartRemoteProcessingModes(VOID *pthis, XUID xuid, int a2, int a3)
{
	LOG_TRACE(voice_log, "StartRemoteProcessingModes() - XUID: {}", xuid);
	return S_OK;
}

HRESULT XHVENGINE::StopRemoteProcessingModes(VOID *pthis, XUID xuidRemoteTalker, /*CONST PXHV_PROCESSING_MODE*/ VOID* a2, int a3)
{
	LOG_TRACE(voice_log, "StopRemoteProcessingModes() - xuidRemoteTalker: {}", xuidRemoteTalker);
	return S_OK;
}

HRESULT XHVENGINE::SetMaxDecodePackets(VOID *pthis, int dwMaxDecodePackets)
{
	return S_OK;
}

HRESULT XHVENGINE::RegisterLocalTalker(VOID *pthis, DWORD dwUserIndex)
{
	return S_OK;
}

HRESULT XHVENGINE::UnregisterLocalTalker(VOID *pthis, DWORD dwUserIndex)
{
	LOG_TRACE(voice_log, "UnregisterLocalTalker()");

	return S_OK;
}

HRESULT XHVENGINE::RegisterRemoteTalker(VOID *pthis, XUID xuid, LPVOID reserved, LPVOID reserved2, LPVOID reserved3)
{
	LOG_TRACE(voice_log, "RegisterRemoteTalker() - XUID: {}", xuid);

	IStreamPlayer* streamPlayer = soundOutput->CreateStreamPlayer(xuid);
	if (streamPlayer)
	{
		/*if (streamPlayer1 == nullptr)
			streamPlayer1 = streamPlayer;*/

		return S_OK;
	}
	
	return E_FAIL;
}


HRESULT XHVENGINE::UnregisterRemoteTalker(VOID *pthis, XUID xuid)
{
	LOG_TRACE(voice_log, "UnregisterRemoteTalker() - XUID: {}", xuid);
	soundOutput->DeleteStreamPlayer(soundOutput->GetStreamPlayer(xuid));

	return S_OK;
}


HRESULT XHVENGINE::GetRemoteTalkers(VOID *pthis, PDWORD pdwRemoteTalkersCount, PXUID pxuidRemoteTalkers)
{
	//LOG_TRACE(voice_log, "[h2mod-voice] - GetRemoteTalkers");
	soundOutput->GetPlayerIdentifiers((int*)pdwRemoteTalkersCount, (long long*)pxuidRemoteTalkers);

	return S_OK;
}

DWORD XHVENGINE::GetDataReadyFlags(VOID *pthis)
{
	LOG_TRACE(voice_log, "GetDataReadyFlags()");
	int ret = 0;

	if (packetBuffer.BytesToRead())
	{
		//LOG_TRACE(voice_log, GetDataReadyFlags()");
		ret |= 1 << 0; // only 1 user with GFWL
	}
	// 0x01 = user 0
	// 0x0F = user 0-3
	return ret;
}

HRESULT XHVENGINE::SubmitIncomingChatData(VOID *pthis, XUID xuidRemoteTalker, const BYTE* pbData, PDWORD pdwSize)
{
	IStreamPlayer* streamPlayer = soundOutput->GetStreamPlayer(xuidRemoteTalker);
	if (streamPlayer != nullptr)
	{
		streamPlayer->PushOpusBuffer(pbData, *pdwSize);
		streamPlayer->Update();
		return S_OK;
	}

	return E_FAIL;
}

HRESULT XHVENGINE::GetLocalChatData(VOID *pthis, DWORD dwUserIndex, PBYTE pbData, PDWORD pdwSize, PDWORD pdwPackets)
{
	// The game uses 10 bytes / voice packet, so we need to split them
	
	if (!pdwSize && !pdwPackets)
	{
		return E_INVALIDARG;
	}

	if (packetBuffer.BytesToRead() > 0)
	{
		memset(pbData, 0, *pdwSize);

		int dataSize = 0;
		packetBuffer.Read((char*)&dataSize, sizeof(int));
		LOG_TRACE(voice_log, "GetLocalChatData() - BytesToRead size: {}", dataSize);

		if (*pdwSize >= dataSize)
			*pdwSize = dataSize;
		else
			*pdwSize = XHV_VOICECHAT_MODE_PACKET_SIZE * XHV_MAX_VOICECHAT_PACKETS; // set to max

		*pdwPackets = (dataSize / XHV_VOICECHAT_MODE_PACKET_SIZE) + (dataSize % XHV_VOICECHAT_MODE_PACKET_SIZE > 0 ? 1 : 0);
		if (*pdwPackets > XHV_MAX_VOICECHAT_PACKETS)
			*pdwPackets = XHV_MAX_VOICECHAT_PACKETS;

		packetBuffer.Read((char*)pbData, dataSize);
		LOG_TRACE(voice_log, "GetLocalChatData() - pdwSize: {}, packet count: {}", *pdwSize, *pdwPackets);

		return S_OK;
	}
	
	return E_PENDING;
}

HRESULT XHVENGINE::SetPlaybackPriority(VOID *pthis, XUID xuidRemoteTalker, DWORD dwUserIndex, int a3)
{
	return S_OK;
}
