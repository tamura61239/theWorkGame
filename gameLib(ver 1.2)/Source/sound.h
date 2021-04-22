#pragma once
#include "sound_buffer.h"

//**************************************
//   音
//**************************************
class Sound
{
public:
	//コンストラクタ
	Sound(const char* name);
	//デストラクタ
	~Sound();
	//更新
	void Update();
	//音を鳴らし始める
	void Play(bool loop = false);
	//音が鳴っているかどうか
	bool Playing();
	//一時停止
	void Pause();
	//停止
	void Stop();
	//音量の設定
	void SetVolume(const float volume);
private:
	IXAudio2* mPXaudio2 = nullptr;
	IXAudio2MasteringVoice* mPMasteringVoice = nullptr;

	std::unique_ptr<SoundBuffer> mSoundBuffer;

	const char* mWavName;

};
