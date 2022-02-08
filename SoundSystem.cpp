#include "Global.h"


SoundSystem* SoundSystem::SSInstance = nullptr;

//intializing xaudio2
SoundSystem* SoundSystem::GetInstance()
{
    if (SSInstance == nullptr)
    {
        SSInstance = new SoundSystem;
    }
    return SSInstance;
}

SoundSystem::SoundSystem():pAudio(NULL), pMasterVoice(NULL), pSourceVoice(NULL)
{
	//INTIALIZING COM
	HRESULT hr;
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        OutputDebugStringA("Failed to initialize COM\n");
    }
		
	//CREATING XAUDIO2 INSTANCE
    if (FAILED(hr = XAudio2Create(&pAudio, 0, XAUDIO2_DEFAULT_PROCESSOR)))
    {
        OutputDebugStringA("Failed to Create XAudio2\n");
    }

	
	//CREATING MASTERING VOICE
    if (FAILED(hr = pAudio->CreateMasteringVoice(&pMasterVoice)))
    {
        OutputDebugStringA("Failed to create Mastering Voice\n");
    }
}

SoundSystem::~SoundSystem()
{
    if (pAudio)
    {
        pAudio->Release();
    }

}

HRESULT SoundSystem::SoundPlay(Sound* object)
{
    HRESULT hr;
    pSourceVoice->SetVolume(0.5);
    if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&object->buffer)))
    {
        OutputDebugStringA("Fail to submit\n");
        return hr;
    }
    //Create SourceVoice for the sourceVoice Pointer
    OutputDebugStringA("SoundPlay\n");
    if (FAILED(hr = pSourceVoice->Start(0)))
    {
        OutputDebugStringA("Fail to start\n");
       return hr;
    }
}
HRESULT SoundSystem::CreateSourceVoice(Sound* object)
{
    HRESULT hr;
    if (FAILED(hr = pAudio->CreateSourceVoice(&pSourceVoice, (WAVEFORMATEX*)&object->wfx)))
    {
        OutputDebugStringA("Fail to create\n");
        return hr;
    }

}
