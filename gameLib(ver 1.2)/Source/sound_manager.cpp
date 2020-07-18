#include"sound_manager.h"
#if SOUNDMODO

SoundManager::SoundManager()
{
	SoundData sounddata[] =
	{
		{ SOUND::BGM_Title,"Data/sounds/result.wav" },
	};
	int size = sizeof(sounddata) / sizeof(SoundData);
	for (int i = 0; i < size; i++)
	{
		mSound.emplace_back(std::make_unique<Sound>(sounddata[i].soundfile));
	}
}
#else
SoundManager::SoundManager()
{
	DirectX::AUDIO_ENGINE_FLAGS flags = DirectX::AudioEngine_Default;

#ifdef _DEBUG
	flags = flags | DirectX::AudioEngine_Debug;
#endif

	audioEngine.reset(new DirectX::AudioEngine(flags));

	SoundData sounddata[] =
	{
		{ SOUND::BGM_Title,L"Data/sounds/result.wav" },
	};
	int size = sizeof(sounddata) / sizeof(SoundData);
	for (int i = 0; i < size; i++)
	{
		mSound.push_back(std::make_unique<Sound>(sounddata[i].soundfile, audioEngine.get()));
	}
}

#endif