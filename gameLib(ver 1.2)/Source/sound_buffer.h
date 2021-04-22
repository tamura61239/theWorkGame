#pragma once
#define SOUNDMODO 1
#if SOUNDMODO
#include <xaudio2.h>

#include <queue>
#include <memory>

//**************************************
//   音のバッファクラス
//**************************************
class SoundBuffer
{
public:
	//コンストラクタ
	SoundBuffer(LPCWSTR szFilename);
	//デストラクタ
	~SoundBuffer() {
		for (size_t i = 0; i < mPSourceVoices.size(); ++i)
		{
			if (mPSourceVoices.at(i))
				mPSourceVoices.at(i)->DestroyVoice();
		}
		mPSourceVoices.clear();
	};
	//音を鳴らし始める
	HRESULT PlayWave(IXAudio2* pXaudio2, bool loop);
	//更新
	void Update();
	//解放
	void Uninitlize();
	//音が鳴っているかどうか
	bool Playing();
	//止める
	void Stop();
	//一時停止
	void Pause();
	//音量の設定
	void Volume(float volume);
private:
	//音データ
	struct WAVData
	{
		const WAVEFORMATEX* wfx;
		const unsigned char* startAudio;
		unsigned int audioBytes;
		unsigned int loopStart;
		unsigned int loopLength;
		const unsigned int* seek;       // Note: XMA Seek data is Big-Endian
		unsigned int seekCount;
	};
	std::unique_ptr<uint8_t[]> mWaveFiles;
	WAVData mWaveData;
	std::vector<IXAudio2SourceVoice*> mPSourceVoices;
	//音が鳴ってるフラグ
	bool mPlaying;
	//ファイルからデータを取得
	HRESULT FindMediaFile(wchar_t* strDestPath, int cchDest, LPCWSTR strFilename);
	HRESULT LoadWAVAudioFromFile(const wchar_t* szFileName, std::unique_ptr<uint8_t[]>& wavData, WAVData& result);
	//初期化
	HRESULT Initialize(LPCWSTR szFilename);
};
#endif