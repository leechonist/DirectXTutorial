#include "soundclass.h"
SoundClass::SoundClass()
{
	m_DirectSound = 0;
	m_primaryBuffer = 0;
	m_secondaryBuffer1 = 0;
}
SoundClass::SoundClass(const SoundClass& other)
{
}
SoundClass::~SoundClass()
{
}

bool SoundClass::Initialize(HWND hwnd)
{
	bool result;
	//Direct Sound API와 1차 버퍼를 초기화
	result = InitializeDirectSound(hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"DS Error", L"Error", MB_OK);
		return false;
	}

	//2차 버퍼에 wave 오디오 파일을 불러옴
	result = LoadWaveFile((char*)"sound01.wav", &m_secondaryBuffer1);
	if (!result)
	{
		MessageBox(hwnd, L"Load File Error", L"Error", MB_OK);
		return false;
	}

	//불러온 wave파일을 재생
	result = PlayWaveFile();
	if (!result)
	{
		MessageBox(hwnd, L"Play Error", L"Error", MB_OK);
		return false;
	}

	return true;
}

void SoundClass::Shutdown()
{
	//2차 버퍼를 해제
	ShutdownWaveFile(&m_secondaryBuffer1);

	//Direct Sound API를 해제
	ShutdownDirectSound();

	return;
}

bool SoundClass::InitializeDirectSound(HWND hwnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;

	//기본 사운드 장치로 사용할 다이렉트 사운드 인터페이스를 초기화
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//협동 레벨 우선 순위를 설정
	result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(result))
	{
		return false;
	}
	
	//1차 버퍼 description을 작성
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	//기본 사운드 장치에 1차 버퍼 제어를 획득
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//1차 사운드 버퍼의 포맷을 변경
	//이 경우에는 초당 44,100샘플을 가진 16비트 스테레오로 녹음 된 .WAV파일로 변경
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	//명시된 포맷으로 1차 버퍼를 설정
	result = m_primaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void SoundClass::ShutdownDirectSound()
{
	//1차 사운드 버퍼 포인터를 해제
	if (m_primaryBuffer)
	{
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}

	//다이렉트 사운드 인터페이스 포인터를 해제
	if (m_DirectSound)
	{
		m_DirectSound->Release();
		m_DirectSound = 0;
	}

	return;
}

bool SoundClass::LoadWaveFile(char* filename, IDirectSoundBuffer8** secondaryBuffer)
{
	int error;
	FILE* filePtr;
	unsigned int count;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char* bufferPtr;
	unsigned long bufferSize;

	//웨이브 파일 열기
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}
	
	//웨이브 파일 헤더 읽기
	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	//Chunk id가 RIFF 포맷인지 확인
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') || (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	//파일 포맷이 Wave 포맷인지 확인
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') || (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}
	
	//Sub chunk ID가 fmt 포맷인지 확인
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') || (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}
	
	//웨이브 파일이 스테레오 파일로 녹음 되었는지 확인
	if (waveFileHeader.numChannels != 2)
	{
		return false;
	}

	//웨이브 파일이 44.1KHZ의 샘플로 녹음 되었는지 확인
	if (waveFileHeader.numChannels != 2)
	{
		return false;
	}

	//웨이브 파일이 16비트 포맷으로 녹음 되었는지 확인
	if (waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	//데이터 청크 헤더를 확인
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') || (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}

	//2차 버퍼의 포맷을 설정
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	//2차 버퍼의 description을 작성
	bufferDesc.dwSize=sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	//명시된 버퍼 설정 대로 임시 버퍼를 생성
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result))
	{
		MessageBox(NULL, L"file", L"Error", MB_OK);
		return false;
	}

	//다이렉트 사운드 8 인터페이스를 기준으로 버퍼 형식을 테스트하고 임시 버퍼 생성
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//임시 버퍼를 해제
	tempBuffer->Release();
	tempBuffer = 0;

	//웨이브 데이터의 시작 부분으로 이동
	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	//웨이브 파일을 고정하기 위해 임시 버퍼를 생성
	waveData = new unsigned char[waveFileHeader.dataSize];
	if (!waveData)
	{
		return false;
	}

	//새로 생성된 버퍼에 웨이브 파일 데이터를 읽어옴
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if (count != waveFileHeader.dataSize)
	{
		return false;
	}

	//한 번 읽기가 끝난 파일을 닫기
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	//2차 버퍼를 작성하기 위해 잠금
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result))
	{
		return false;
	}

	//Wave데이터를 버퍼에 복사
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	//2차 버퍼를 다 작성 했으면 잠금 해제
	if (FAILED(result))
	{
		return false;
	}

	//2차 버퍼에 복사가 되었다면 웨이브 데이터를 해제
	delete[] waveData;
	waveData = 0;

	return true;
}

void SoundClass::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer)
{
	//2차 버퍼를 해제
	if (*secondaryBuffer)
	{
		(*secondaryBuffer)->Release();
		*secondaryBuffer = 0;
	}
	return;
}

bool SoundClass::PlayWaveFile()
{
	HRESULT result;
	//사운드 버퍼의 시작 지점을 설정
	result = m_secondaryBuffer1->SetCurrentPosition(0);
	if (FAILED(result))
	{
		return false;
	}
	
	//볼륨을 100%로 설정
	result = m_secondaryBuffer1->SetVolume(DSBVOLUME_MAX);
	if (FAILED(result))
	{
		return false;
	}

	//2차 사운드 버퍼에 있는 내용물을 재생
	result = m_secondaryBuffer1->Play(0, 0, DSBPLAY_LOOPING);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}