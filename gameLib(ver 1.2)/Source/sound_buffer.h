#pragma once
#define SOUNDMODO 1
#if SOUNDMODO
//**************************************
//   include Headers
//**************************************
#include <xaudio2.h>

#include <queue>
#include <memory>

//**************************************
//   Object class
//**************************************
class SoundBuffer
{
public:
	SoundBuffer(LPCWSTR szFilename);
	~SoundBuffer() {
		for (size_t i = 0; i < mPSourceVoices.size(); ++i)
		{
			if (mPSourceVoices.at(i))
				mPSourceVoices.at(i)->DestroyVoice();
		}
		mPSourceVoices.clear();
	};

	HRESULT PlayWave(IXAudio2* pXaudio2, bool loop);
	void Update();
	void Uninitlize();
	bool Playing();
	void Stop();
	void Pause();
	void Volume(float volume);
private:
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
	std::vector<IXAudio2SourceVoice*> mPSourceVoices;
	std::unique_ptr<uint8_t[]> mWaveFiles;
	WAVData mWaveData;

	bool mPlaying;

	HRESULT FindMediaFile(wchar_t* strDestPath, int cchDest, LPCWSTR strFilename);
	HRESULT LoadWAVAudioFromFile(const wchar_t* szFileName, std::unique_ptr<uint8_t[]>& wavData, WAVData& result);
	HRESULT Initialize(LPCWSTR szFilename);
};
#endif