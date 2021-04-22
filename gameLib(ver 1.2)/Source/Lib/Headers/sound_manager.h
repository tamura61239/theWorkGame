#pragma once
#include"sound.h"
//鳴らせる音の管理クラス
class SoundManager
{
public:
	//鳴らし始める
	void Play(int soundNo, bool loop = false)
	{
		mSound.at(soundNo)->Play(loop);
	}
	//停止
	void Stop(int soundNo)
	{
		mSound.at(soundNo)->Pause();
	}
	//更新
	void Update()
	{
		for (auto& sound : mSound)
		{
			sound->Update();
		}
	}
	//音量設定
	void SetVolume(int soundNo, const float volume)
	{
		mSound.at(soundNo)->SetVolume(volume);
	}
	//デストラクタ
	~SoundManager()
	{
		mSound.clear();
	}
	//音の種類と配列番号
	struct SoundData
	{
		int soundNo;
		const char* soundfile;
	};
	//音の種類
	enum SOUND
	{
		BGM_Title,
		BGM_Loading,
		BGM_Game,
		BGM_CLEAR,
		BGM_OVER,
		max
	};
	//インスタンス
	static SoundManager& getinctance()
	{
		static SoundManager s;
		return s;
	}
private:
	//音のリスト
	std::vector<std::unique_ptr<Sound>>mSound;
	//コンストラクタ
	SoundManager();

};
#define pSoundManager (SoundManager::getinctance())

