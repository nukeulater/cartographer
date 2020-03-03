#pragma once
#include "IStreamPlayer.h"
#include "VoiceError.h"

class ISoundOutput
{
public:
	virtual ~ISoundOutput() = default;

	virtual void SetMyPosition(float x, float y, float z) = 0;
	virtual void SetMyVelocity(float x, float y, float z) = 0;
	virtual void SetMyOrientationFront(float x, float y, float z) = 0;
	virtual void SetMyOrientationUp(float x, float y, float z) = 0;
	virtual void UpdateMe() = 0;

	virtual IStreamPlayer* CreateStreamPlayer(long long identifier) = 0;
	virtual IStreamPlayer* GetStreamPlayer(long long identifier) = 0;
	virtual void GetPlayerIdentifiers(int* identifierCount, long long* identifiers) = 0;
	virtual void DeleteStreamPlayer(IStreamPlayer* streamPlayer) = 0;
	virtual void SetBufferingTime(unsigned int timeMS) = 0;
	virtual void SetExtraGain(float gain) = 0;
	virtual void UpdateAllStreamPlayers() = 0;

	virtual AltVoiceError ChangeDevice(const char* deviceName) = 0;
	virtual AltVoiceError GetLastError() = 0;
};
