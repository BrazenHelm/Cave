#include "AudioPlayer.h"


bool AudioPlayer::Initialize(HWND hWnd, const std::string& path)
{
	// Initialize DirectSound and the primary buffer
	HRESULT result = DirectSoundCreate8(NULL, &m_pSound, NULL);
	if (FAILED(result)) return false;

	result = m_pSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
	if (FAILED(result)) return false;

	DSBUFFERDESC bd = {};
	bd.dwSize = sizeof(DSBUFFERDESC);
	bd.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bd.dwBufferBytes = 0;
	bd.dwReserved = 0;
	bd.lpwfxFormat = NULL;
	bd.guid3DAlgorithm = GUID_NULL;

	result = m_pSound->CreateSoundBuffer(&bd, &m_pPrimaryBuffer, NULL);
	if (FAILED(result)) return false;

	WAVEFORMATEX wf = {};
	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nSamplesPerSec = 44100;
	wf.wBitsPerSample = 16;
	wf.nChannels = 2;
	wf.nBlockAlign = (wf.wBitsPerSample / 8) * wf.nChannels;
	wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
	wf.cbSize = 0;

	result = m_pPrimaryBuffer->SetFormat(&wf);
	if (FAILED(result)) return false;

	// Load .wav file to the secondary buffer
	if (!LoadWAV(path)) return false;

	return true;
}


void AudioPlayer::Play(bool looping)
{
	m_pSecondaryBuffer->SetCurrentPosition(0);
	m_looping = looping;
	DWORD flags = (looping) ? DSBPLAY_LOOPING : 0;
	m_pSecondaryBuffer->Play(0, 0, flags);
}


void AudioPlayer::IncreaseVolume()
{
	if (m_volume < DSBVOLUME_MAX)
	{
		m_volume += 500;
		m_pSecondaryBuffer->SetVolume(m_volume);
	}
}


void AudioPlayer::DecreaseVolume()
{
	if (m_volume > DSBVOLUME_MIN)
	{
		m_volume -= 500;
		m_pSecondaryBuffer->SetVolume(m_volume);
	}
}


bool AudioPlayer::LoadWAV(const std::string& path)
{
	FILE* pFile;
	int error = fopen_s(&pFile, path.c_str(), "rb");
	if (error != 0) return false;

	//char headerString[512];
	WAVHeaderType header;
	size_t count = fread(&header, sizeof(WAVHeaderType), 1, pFile);
	//size_t count = fread(headerString, 1, 512, pFile);
	if (count != 1) return false;

	// check that the settings are fine
	if (header.ChunkID[0] != 'R' || header.ChunkID[1] != 'I' ||
		header.ChunkID[2] != 'F' || header.ChunkID[3] != 'F')
		return false;
	if (header.Format[0] != 'W' || header.Format[1] != 'A' ||
		header.Format[2] != 'V' || header.Format[3] != 'E')
		return false;
	if (header.SubchunkFormatID[0] != 'f' || header.SubchunkFormatID[1] != 'm' ||
		header.SubchunkFormatID[2] != 't' || header.SubchunkFormatID[3] != ' ')
		return false;
	if (header.AudioFormat != WAVE_FORMAT_PCM) return false;
	if (header.NumChannels != 2) return false;
	if (header.SampleRate != 44100) return false;
	if (header.BitsPerSample != 16) return false;
	/*if (header.SubchunkDataID[0] != 'd' || header.SubchunkDataID[1] != 'a' ||
		header.SubchunkDataID[2] != 't' || header.SubchunkDataID[3] != 'a')
		return false;*/

	// Set up secondary buffer

	WAVEFORMATEX wf = {};
	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nSamplesPerSec = 44100;
	wf.wBitsPerSample = 16;
	wf.nChannels = 2;
	wf.nBlockAlign = (wf.wBitsPerSample / 8) * wf.nChannels;
	wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
	wf.cbSize = 0;

	const unsigned long dataSize = header.ChunkSize - sizeof(header);

	DSBUFFERDESC bd = {};
	bd.dwSize = sizeof(DSBUFFERDESC);
	bd.dwFlags = DSBCAPS_CTRLVOLUME;
	bd.dwBufferBytes = dataSize;
	bd.dwReserved = 0;
	bd.lpwfxFormat = &wf;
	bd.guid3DAlgorithm = GUID_NULL;

	IDirectSoundBuffer* pTempBuffer = nullptr;
	HRESULT result = m_pSound->CreateSoundBuffer(&bd, &pTempBuffer, NULL);
	if (FAILED(result)) return false;

	result = pTempBuffer->QueryInterface(
		IID_IDirectSoundBuffer8, &m_pSecondaryBuffer);
	if (FAILED(result)) return false;

	pTempBuffer->Release();
	pTempBuffer = nullptr;

	// Load data from file

	fseek(pFile, sizeof(WAVHeaderType), SEEK_SET);
	unsigned char* data = new unsigned char[dataSize];
	if (!data) return false;

	count = fread(data, 1, dataSize, pFile);
	if (count != dataSize) return false;

	error = fclose(pFile);
	if (error != 0) return false;

	unsigned char* pBuffer;
	unsigned long bufferSize;
	result = m_pSecondaryBuffer->Lock(0, dataSize, (void**)&pBuffer,
		(DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result)) return false;

	memcpy(pBuffer, data, dataSize);

	result = m_pSecondaryBuffer->Unlock((void*)pBuffer, bufferSize, NULL, 0);
	if (FAILED(result)) return false;

	delete[] data;
	data = nullptr;

	result = m_pSecondaryBuffer->SetVolume(DSBVOLUME_MAX);
	if (FAILED(result)) return false;

	return true;
}

