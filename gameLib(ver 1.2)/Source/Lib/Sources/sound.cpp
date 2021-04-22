#include "sound.h"
//**************************************
//   include Headers
//**************************************
#include "sound.h"
#include "logger.h"

#include <assert.h>

/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/

Sound::Sound(const char* filename)
{
	wchar_t wav_name_w[256];
	mbstowcs_s(0, wav_name_w, filename, strlen(filename) + 1);
	mSoundBuffer = std::make_unique<SoundBuffer>(wav_name_w);

	//
	// Initialize XAudio2
	//
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	UINT32 flags = 0;
#if (_WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(_DEBUG)
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif
	HRESULT hr = XAudio2Create(&mPXaudio2, flags);
	if (FAILED(hr))
	{
		CoUninitialize();
		//LOGGER("******************************\n");
		//LOGGER("XAudio2Create() error  (Sound)\n");
		//LOGGER("******************************\n");
		assert(!"Could not Create XAudio2");
	}

#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(_DEBUG)
	// To see the trace output, you need to view ETW logs for this application:
	//    Go to Control Panel, Administrative Tools, Event Viewer.
	//    View->Show Analytic and Debug Logs.
	//    Applications and Services Logs / Microsoft / Windows / XAudio2. 
	//    Right click on Microsoft Windows XAudio2 debug logging, Properties, then Enable Logging, and hit OK 
	XAUDIO2_DEBUG_CONFIGURATION debug = { 0 };
	debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
	debug.BreakMask = XAUDIO2_LOG_ERRORS;
	mPXaudio2->SetDebugConfiguration(&debug, 0);
#endif

	//
	// Create a mastering voice
	//
	hr = mPXaudio2->CreateMasteringVoice(&mPMasteringVoice);
	if (FAILED(hr))
	{
		wprintf(L"Failed creating mastering voice: %#X\n", hr);
		if (mPXaudio2 != nullptr)
		{
			mPXaudio2->Release();
			mPXaudio2 = nullptr;
		}
		CoUninitialize();
		//LOGGER("*************************************\n");
		//LOGGER("CreateMasteringVoice() error  (Sound)\n");
		//LOGGER("*************************************\n");
		assert(!"Could not Create MasteringVoice");
	}
}
/*****************************************************/
//　　　　　　　　　　解放関数(デストラクタ)
/*****************************************************/

Sound::~Sound()
{
	mSoundBuffer->Uninitlize();

	if (mPXaudio2 != nullptr)
	{
		if (mPMasteringVoice != nullptr)
		{
			mPMasteringVoice->DestroyVoice();
			mPMasteringVoice = nullptr;
		}

		mPXaudio2->Release();
		mPXaudio2 = nullptr;
	}
	CoUninitialize();
}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/

void Sound::Update()
{
	mSoundBuffer->Update();
}
/*********************音を鳴らし始める***********************/
void Sound::Play(bool loop)
{
	HRESULT hr = mSoundBuffer->PlayWave(mPXaudio2, loop);
	if (FAILED(hr))
	{
		//LOGGER("*************************\n");
		//LOGGER("PlayWave() error  (Sound)\n");
		//LOGGER("*************************\n");
		assert(!"Could not Play sound");
	}
}
/*******************音が鳴っているかどうか*******************/
bool Sound::Playing()
{

	return mSoundBuffer->Playing();
}
/******************一時停止******************/
void Sound::Pause()
{
	mSoundBuffer->Pause();
}
/****************停止***************/
void Sound::Stop()
{
	mSoundBuffer->Stop();
}
/*********************音の大きさを設定する********************/
void Sound::SetVolume(const float volume)
{
	mSoundBuffer->Volume(volume);
}
