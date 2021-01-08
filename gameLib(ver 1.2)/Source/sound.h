#pragma once
//**************************************
//   include Headers
//**************************************
#include "sound_buffer.h"

#if SOUNDMODO
//**************************************
//   Object class
//**************************************
class Sound
{
public:
	Sound(const char* name);
	~Sound();

	void Update();
	void Play(bool loop = false);
	bool Playing();
	void Pause();
	void Stop();
	void SetVolume(const float volume);
private:
	IXAudio2* mPXaudio2 = nullptr;
	IXAudio2MasteringVoice* mPMasteringVoice = nullptr;

	std::unique_ptr<SoundBuffer> mSoundBuffer;

	const char* mWavName;

};
#else
#include<memory>
#include<Audio.h>

class Sound
{
public:
	Sound(const wchar_t* fileName, DirectX::AudioEngine*audioEngine);
	void Play(bool loop = false);
	bool SoundMove();
	void Stop();
	void SetVolume(float volume);
	~Sound(){}
private:
	std::unique_ptr<DirectX::SoundEffect>music;
	std::unique_ptr<DirectX::SoundEffectInstance>musicInstance;
};
#endif