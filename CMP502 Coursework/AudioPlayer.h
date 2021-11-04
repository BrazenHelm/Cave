#pragma once

#include <WindowsInclude.h>
#include "Inputs.h"
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include <string>
#include <wrl.h>

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

class AudioPlayer
{
private:
	struct WAVHeaderType
	{
		char			ChunkID[4];
		unsigned long	ChunkSize;
		char			Format[4];
		char			SubchunkFormatID[4];
		unsigned long	SubchunkFormatSize;
		unsigned short	AudioFormat;
		unsigned short	NumChannels;
		unsigned long	SampleRate;
		unsigned long	ByteRate;
		unsigned short	BlockAlign;
		unsigned short	BitsPerSample;
		char			Info[130];				// information about artist etc.
		char			SubchunkDataID[4];
		unsigned long	SubchunkDataSize;
	};

public:
	bool Initialize(HWND hWnd, const std::string& path);
	void Play(bool looping);
	
	void IncreaseVolume();
	void DecreaseVolume();

private:
	bool LoadWAV(const std::string& path);
	LONG m_volume = DSBVOLUME_MAX;
	bool m_looping = false;

private:
	Microsoft::WRL::ComPtr<IDirectSound8>		m_pSound;
	Microsoft::WRL::ComPtr<IDirectSoundBuffer>	m_pPrimaryBuffer;
	Microsoft::WRL::ComPtr<IDirectSoundBuffer8>	m_pSecondaryBuffer;
};

