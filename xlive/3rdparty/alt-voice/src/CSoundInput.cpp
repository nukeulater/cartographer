#include <chrono>
#include "CSoundInput.h"
#include "CVoiceException.h"

CSoundInput::CSoundInput(char* deviceName, int sampleRate, int framesPerBuffer, int bitrate) : _sampleRate(sampleRate), _framesPerBuffer(framesPerBuffer), _bitRate(bitrate)
{
	// Nuke: for whatever reason I cannot use FLOAT32 format
	//if (alIsExtensionPresent("AL_EXT_float32") != AL_TRUE)
		//throw CVoiceException(AltVoiceError::DeviceOpenError);

	inputDevice = alcCaptureOpenDevice(deviceName, sampleRate, AL_FORMAT_MONO16, framesPerBuffer);
	if (!inputDevice)
		throw CVoiceException(AltVoiceError::DeviceOpenError);

	inputStreamThread = new std::thread(&CSoundInput::OnVoiceInput, this);
	inputStreamThread->detach();

	sleepTime = (framesPerBuffer / (sampleRate / 1000)) / 2;

	int opusErr;
	enc = opus_encoder_create(sampleRate, 1, OPUS_APPLICATION_VOIP, &opusErr);
	if (opusErr != OPUS_OK || enc == NULL)
		throw CVoiceException(AltVoiceError::OpusEncoderCreateError);

	if (opus_encoder_ctl(enc, OPUS_SET_BITRATE(_bitRate)) != OPUS_OK)
		throw CVoiceException(AltVoiceError::OpusBitrateSetError);

	transferBuffer = new Sample[framesPerBuffer];
}

CSoundInput::~CSoundInput()
{
	threadAlive = false;
	delete inputStreamThread;

	alcCaptureCloseDevice(inputDevice);
	opus_encoder_destroy(enc);
}

void CSoundInput::OnVoiceInput()
{
	static uint32_t catchedFrames = 0;
	static Sample opusFrameBuffer[FRAME_SIZE_OPUS];
	static unsigned char packet[MAX_PACKET_SIZE];
	static bool isBufferCaptured = false;

	while(threadAlive)
	{
		isBufferCaptured = false;
		{
			bool locked = deviceMutex.try_lock();
			if (locked)
			{
				if (!inputDevice)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(5));
					continue;
				}
				alcGetIntegerv(inputDevice, ALC_CAPTURE_SAMPLES, sizeof(ALCint), (ALCint*)&catchedFrames);
				if (catchedFrames >= _framesPerBuffer)
				{
					alcCaptureSamples(inputDevice, transferBuffer, _framesPerBuffer);
					isBufferCaptured = true;
				}
				deviceMutex.unlock();
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
				continue;
			}
		}

		if(isBufferCaptured)
		{
			int micLevel = 0;
			for (uint32_t i = 0; i < _framesPerBuffer; ++i)
			{
				if (std::abs(transferBuffer[i]) > micLevel)
					micLevel = std::abs(transferBuffer[i]);
			}
			
			{
				std::unique_lock<std::mutex> _micGainLock(micGainMutex);
				for (uint32_t i = 0; i < _framesPerBuffer; ++i)
					transferBuffer[i] = transferBuffer[i] * micGain;
			}
			
			_ringBuffer.Write((const Sample*)transferBuffer, _framesPerBuffer);

			while (_ringBuffer.BytesToRead() >= FRAME_SIZE_OPUS)
			{
				_ringBuffer.Read((Sample*)opusFrameBuffer, FRAME_SIZE_OPUS);
				int len = opus_encode(enc, opusFrameBuffer, FRAME_SIZE_OPUS, packet, MAX_PACKET_SIZE);

				if (len < 0 || len > MAX_PACKET_SIZE)
				{
					threadAlive = false;
					return;
				}

				if (cb)
					cb(packet, len, micLevel);
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
	}
}

bool CSoundInput::EnableInput()
{
	if (!inputActive)
	{
		if (inputDevice)
		{
			inputActive = true;
			alcCaptureStart(inputDevice);
			return true;
		}
	}
	return false;
}

bool CSoundInput::DisableInput()
{
	if (inputActive)
	{
		inputActive = false;
		if(inputDevice)
			alcCaptureStop(inputDevice);
		return true;
	}
	return false;
}

void CSoundInput::ChangeMicGain(float gain)
{
	std::unique_lock<std::mutex> _micGainLock(micGainMutex);
	micGain = gain;
}

bool CSoundInput::ChangeDevice(char * deviceName)
{
	std::unique_lock<std::mutex> _deviceLock(deviceMutex);
	alcCaptureCloseDevice(inputDevice);

	inputDevice = alcCaptureOpenDevice(deviceName, _sampleRate, AL_FORMAT_MONO16, _framesPerBuffer);
	if (!inputDevice)
		return false;
	return true;
}

void CSoundInput::RegisterCallback(OnVoiceCallback callback)
{
	cb = callback;
}

float CSoundInput::GetBufferFillLevel()
{
	return _ringBuffer.GetFillLevel();
}

void CSoundInput::Lock()
{
	deviceMutex.lock();
}

bool CSoundInput::TryLock()
{
	return deviceMutex.try_lock();
}

void CSoundInput::Unlock()
{
	deviceMutex.unlock();
}
