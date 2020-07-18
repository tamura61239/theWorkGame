#pragma once
#include"sound.h"
#if SOUNDMODO
class SoundManager
{
public:
	void Play(int soundNo, bool loop = false)
	{
		mSound.at(soundNo)->Play(loop);
	}
	void Stop(int soundNo)
	{
		mSound.at(soundNo)->Pause();
	}
	void Update()
	{
		for (auto& sound : mSound)
		{
			sound->Update();
		}
	}
	void SetVolume(int soundNo, const float volume)
	{
		mSound.at(soundNo)->SetVolume(volume);
	}
	~SoundManager()
	{
		mSound.clear();
	}
	struct SoundData
	{
		int soundNo;
		const char* soundfile;
	};
	enum SOUND
	{
		BGM_Title,
		BGM_Loading,
		BGM_Game,
		BGM_CLEAR,
		BGM_OVER,
		max
	};
	static SoundManager& getinctance()
	{
		static SoundManager s;
		return s;
	}
private:
	std::vector<std::unique_ptr<Sound>>mSound;
	SoundManager();

};
#define pSoundManager (SoundManager::getinctance())

#else
#include<vector>

class SoundManager
{
public:
	static SoundManager& GetInctance()
	{
		static SoundManager manager;
		return manager;
	}
	void Play(int soundNum, bool loop = false) { mSound[soundNum]->Play(loop); }
	void Stop(int soundNum) { mSound[soundNum]->Stop(); }
	bool SoundMove(int soundNum) { return mSound[soundNum]->SoundMove(); }
	void SetVolume(int soundNum,float volume) { mSound[soundNum]->SetVolume(volume); }
	enum SOUND
	{
		BGM_Title,
		BGM_Loading,
		BGM_Game,
		BGM_CLEAR,
		BGM_OVER,
		max
	};
private:
	struct SoundData
	{
		int soundNo;
		const wchar_t* soundfile;
	};
	SoundManager();
	std::unique_ptr<DirectX::AudioEngine> audioEngine;
	std::vector<std::unique_ptr<Sound>>mSound;
};
#define pSoundManager (SoundManager::GetInctance())

#endif
