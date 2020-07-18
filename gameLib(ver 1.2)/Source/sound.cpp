#include "sound.h"
#if SOUNDMODO
//**************************************
//   include Headers
//**************************************
#include "sound.h"
#include "logger.h"

#include <assert.h>


Sound::Sound(const char* filename)
{
	wchar_t wav_name_w[256];
	mbstowcs_s(0, wav_name_w, filename, strlen(filename) + 1);
	sound_buffer = std::make_unique<SoundBuffer>(wav_name_w);

	//
	// Initialize XAudio2
	//
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	UINT32 flags = 0;
#if (_WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(_DEBUG)
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif
	HRESULT hr = XAudio2Create(&m_p_xaudio2, flags);
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
	m_p_xaudio2->SetDebugConfiguration(&debug, 0);
#endif

	//
	// Create a mastering voice
	//
	hr = m_p_xaudio2->CreateMasteringVoice(&m_p_mastering_voice);
	if (FAILED(hr))
	{
		wprintf(L"Failed creating mastering voice: %#X\n", hr);
		if (m_p_xaudio2 != nullptr)
		{
			m_p_xaudio2->Release();
			m_p_xaudio2 = nullptr;
		}
		CoUninitialize();
		//LOGGER("*************************************\n");
		//LOGGER("CreateMasteringVoice() error  (Sound)\n");
		//LOGGER("*************************************\n");
		assert(!"Could not Create MasteringVoice");
	}
}

Sound::~Sound()
{
	sound_buffer->Uninitlize();

	if (m_p_xaudio2 != nullptr)
	{
		if (m_p_mastering_voice != nullptr)
		{
			m_p_mastering_voice->DestroyVoice();
			m_p_mastering_voice = nullptr;
		}

		m_p_xaudio2->Release();
		m_p_xaudio2 = nullptr;
	}
	CoUninitialize();
}

void Sound::Update()
{
	sound_buffer->Update();
}

void Sound::Play(bool loop)
{
	HRESULT hr = sound_buffer->PlayWave(m_p_xaudio2, loop);
	if (FAILED(hr))
	{
		//LOGGER("*************************\n");
		//LOGGER("PlayWave() error  (Sound)\n");
		//LOGGER("*************************\n");
		assert(!"Could not Play sound");
	}
}
bool Sound::Playing()
{

	return sound_buffer->Playing();
}

void Sound::Pause()
{
	sound_buffer->Pause();
}

void Sound::Stop()
{
	sound_buffer->Stop();
}

void Sound::SetVolume(const float volume)
{
	sound_buffer->Volume(volume);
}
#else
Sound::Sound(const wchar_t* fileName, DirectX::AudioEngine* audioEngine)
{
	music.reset(new DirectX::SoundEffect(audioEngine, fileName));
	musicInstance = music->CreateInstance();
}

void Sound::Play(bool loop)
{
	musicInstance->Play(loop);
}
bool Sound::SoundMove()
{
	
	if (musicInstance->GetState() == DirectX::SoundState::STOPPED)return false;
	return true;
}

void Sound::Stop()
{
	musicInstance->Pause();
	musicInstance->IsLooped();
}

void Sound::SetVolume(float volume)
{
	musicInstance->SetVolume(volume);
}
#endif
