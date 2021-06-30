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
	//Direct Sound API�� 1�� ���۸� �ʱ�ȭ
	result = InitializeDirectSound(hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"DS Error", L"Error", MB_OK);
		return false;
	}

	//2�� ���ۿ� wave ����� ������ �ҷ���
	result = LoadWaveFile((char*)"sound01.wav", &m_secondaryBuffer1);
	if (!result)
	{
		MessageBox(hwnd, L"Load File Error", L"Error", MB_OK);
		return false;
	}

	//�ҷ��� wave������ ���
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
	//2�� ���۸� ����
	ShutdownWaveFile(&m_secondaryBuffer1);

	//Direct Sound API�� ����
	ShutdownDirectSound();

	return;
}

bool SoundClass::InitializeDirectSound(HWND hwnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;

	//�⺻ ���� ��ġ�� ����� ���̷�Ʈ ���� �������̽��� �ʱ�ȭ
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//���� ���� �켱 ������ ����
	result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(result))
	{
		return false;
	}
	
	//1�� ���� description�� �ۼ�
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	//�⺻ ���� ��ġ�� 1�� ���� ��� ȹ��
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//1�� ���� ������ ������ ����
	//�� ��쿡�� �ʴ� 44,100������ ���� 16��Ʈ ���׷����� ���� �� .WAV���Ϸ� ����
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	//��õ� �������� 1�� ���۸� ����
	result = m_primaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void SoundClass::ShutdownDirectSound()
{
	//1�� ���� ���� �����͸� ����
	if (m_primaryBuffer)
	{
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}

	//���̷�Ʈ ���� �������̽� �����͸� ����
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

	//���̺� ���� ����
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}
	
	//���̺� ���� ��� �б�
	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	//Chunk id�� RIFF �������� Ȯ��
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') || (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	//���� ������ Wave �������� Ȯ��
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') || (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}
	
	//Sub chunk ID�� fmt �������� Ȯ��
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') || (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}
	
	//���̺� ������ ���׷��� ���Ϸ� ���� �Ǿ����� Ȯ��
	if (waveFileHeader.numChannels != 2)
	{
		return false;
	}

	//���̺� ������ 44.1KHZ�� ���÷� ���� �Ǿ����� Ȯ��
	if (waveFileHeader.numChannels != 2)
	{
		return false;
	}

	//���̺� ������ 16��Ʈ �������� ���� �Ǿ����� Ȯ��
	if (waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	//������ ûũ ����� Ȯ��
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') || (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}

	//2�� ������ ������ ����
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	//2�� ������ description�� �ۼ�
	bufferDesc.dwSize=sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	//��õ� ���� ���� ��� �ӽ� ���۸� ����
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result))
	{
		MessageBox(NULL, L"file", L"Error", MB_OK);
		return false;
	}

	//���̷�Ʈ ���� 8 �������̽��� �������� ���� ������ �׽�Ʈ�ϰ� �ӽ� ���� ����
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//�ӽ� ���۸� ����
	tempBuffer->Release();
	tempBuffer = 0;

	//���̺� �������� ���� �κ����� �̵�
	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	//���̺� ������ �����ϱ� ���� �ӽ� ���۸� ����
	waveData = new unsigned char[waveFileHeader.dataSize];
	if (!waveData)
	{
		return false;
	}

	//���� ������ ���ۿ� ���̺� ���� �����͸� �о��
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if (count != waveFileHeader.dataSize)
	{
		return false;
	}

	//�� �� �бⰡ ���� ������ �ݱ�
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	//2�� ���۸� �ۼ��ϱ� ���� ���
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result))
	{
		return false;
	}

	//Wave�����͸� ���ۿ� ����
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	//2�� ���۸� �� �ۼ� ������ ��� ����
	if (FAILED(result))
	{
		return false;
	}

	//2�� ���ۿ� ���簡 �Ǿ��ٸ� ���̺� �����͸� ����
	delete[] waveData;
	waveData = 0;

	return true;
}

void SoundClass::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer)
{
	//2�� ���۸� ����
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
	//���� ������ ���� ������ ����
	result = m_secondaryBuffer1->SetCurrentPosition(0);
	if (FAILED(result))
	{
		return false;
	}
	
	//������ 100%�� ����
	result = m_secondaryBuffer1->SetVolume(DSBVOLUME_MAX);
	if (FAILED(result))
	{
		return false;
	}

	//2�� ���� ���ۿ� �ִ� ���빰�� ���
	result = m_secondaryBuffer1->Play(0, 0, DSBPLAY_LOOPING);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}