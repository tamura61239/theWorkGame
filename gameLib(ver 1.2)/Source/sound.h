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
	IXAudio2* m_p_xaudio2 = nullptr;
	IXAudio2MasteringVoice* m_p_mastering_voice = nullptr;

	std::unique_ptr<SoundBuffer> sound_buffer;

	const char* wav_name;

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