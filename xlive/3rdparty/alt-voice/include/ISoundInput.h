#pragma once

using OnVoiceCallback = void(*)(const void* buffer, int size, short micLevel);
using UpdateCallback = void(*)();

class ISoundInput
{
public:
	virtual ~ISoundInput() = default;

	virtual bool EnableInput() = 0;
	virtual bool DisableInput() = 0;
	virtual void ChangeMicGain(float gain) = 0;
	virtual bool ChangeDevice(char* deviceName) = 0;
	virtual void RegisterCallback(OnVoiceCallback callback) = 0;
	virtual float GetBufferFillLevel() = 0;
	virtual void Lock() = 0;
	virtual void Unlock() = 0;
	virtual bool TryLock() = 0;
};
