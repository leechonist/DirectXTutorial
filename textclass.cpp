#include "textclass.h"
TextClass::TextClass()
{
	m_Font = 0;
	m_FontShader = 0;
	m_sentence1 = 0;
	m_sentence2 = 0;
}

TextClass::TextClass(const TextClass& other)
{
}

TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, D3DXMATRIX baseViewMatrix)
{
	bool result;

	//화면 높이, 넓이 저장
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	//기본 뷰 행렬 저장
	m_baseViewMatrix = baseViewMatrix;

	//폰트 객체 생성
	m_Font = new FontClass;
	if (!m_Font)
	{
		return false;
	}

	//폰트 객체 초기화
	result = m_Font->Initialize(device, (char*)"fontdata.txt", L"font.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	//폰트쉐이더 객체 생성
	m_FontShader = new FontShaderClass;
	if (!m_FontShader)
	{
		return false;
	}

	//폰트 쉐이더를 해제
	result = m_FontShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initailize the font shader object.", L"Error", MB_OK);
		return false;
	}

	//첫번째 문장을 초기화
	result = InitializeSentence(&m_sentence1, 16, device);
	if (!result)
	{
		return false;
	}

	//문장 정점 버퍼를 새 정보와 함께 초기화
	result = UpdateSentence(m_sentence1, (char*)"Hello", 100, 100, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	//두 번째 문장을 초기화
	result = InitializeSentence(&m_sentence2, 16, device);
	if (!result)
	{
		return false;
	}

	//문장 정점 버퍼를 새 정보와 함께 초기화
	result = UpdateSentence(m_sentence2, (char*)"Goodbye", 100, 200, 1.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

void TextClass::Shutdown()
{
	//첫번째 문장 해제
	ReleaseSentence(&m_sentence1);

	//두 번째 문장 해제
	ReleaseSentence(&m_sentence2);

	//폰트 쉐이더 객체 해제
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	//폰트 객체 해제
	if (m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}

	return;
}

bool TextClass::Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
	bool result;

	//첫번째 문장을 그림
	result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	//두 번째 문장을 그림
	result = RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	//새로운 문장 객체를 생성
	*sentence = new SentenceType;
	if (!*sentence)
	{
		return false;
	}

	//문장 버퍼를 null로 초기화
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	//문장의 최대 길이를 설정
	(*sentence)->maxLength = maxLength;

	//정점 배열의 갯수를 설정
	(*sentence)->vertexCount = 6 * maxLength;
	
	//인덱스 배열의 갯수를 설정
	(*sentence)->indexCount = (*sentence)->vertexCount;

	//정점 배열 생성
	vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}
	
	//인덱스 배열 생성
	indices = new unsigned long[(*sentence)->indexCount];
	if (!indices)
	{
		return false;
	}

	//정점 배열을 0으로 초기화
	memset(vertices, 0, (sizeof(VertexType) * ((*sentence)->vertexCount)));

	//인덱스 배열을 초기화
	for (i = 0; i < (*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	//동적 정점 버퍼를 설정
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//??
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//정점 버퍼 생성
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//정적 인덱스 버퍼 디스크립션 설정
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//??
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//인덱스 버퍼 생성
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//정점 배열을 해제
	delete [] vertices;
	vertices = 0;

	//인덱스 배열을 해제
	delete[] indices;
	indices = 0;

	return true;
}

bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue, ID3D11DeviceContext* deviceContext)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	//문장의 색을 저장
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	//문장의 길이를 설정
	numLetters = (int)strlen(text);

	//오버플로우가 발생했는지 체크
	if (numLetters > sentence->maxLength)
	{
		return false;
	}

	//정점 배열을 생성
	vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	//정점 배열을 0으로 초기화
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	//그려질 X,Y 위치를 계산
	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);

	//폰트 클래스를 사용해 정점 배열을 생성
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	//정점 버퍼를 잠금
	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//정점 버퍼 포인터 저장
	verticesPtr = (VertexType*)mappedResource.pData;

	//데이터를 정점 버퍼에 복사
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	//정점버퍼 잠금 해제
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	//정점 배열 해제
	delete[] vertices;
	vertices = 0;

	return true;
}

void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence)
	{
		//정점 버퍼를 해제
		if ((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		//인덱스 버퍼를 해제
		if ((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		//문장을 해제
		delete* sentence;
		*sentence = 0;
	}

	return;
}

bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
	unsigned int stride, offset;
	D3DXVECTOR4 pixelColor;
	bool result;

	//정점 버퍼의 stride와 offset을 설정
	stride = sizeof(VertexType);
	offset = 0;

	//정점 버퍼를 렌더링 할 수 있도록 어셈블러에 넣어 활성화
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	//인덱스 버퍼를 렌더링 할 수 있도록 어셈블러에 넣어 활성화
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//정점 버퍼가 그려낼 도형을 설정 여기서는 삼각형
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//픽셀 색상 벡터를 생성
	pixelColor = D3DXVECTOR4(sentence->red, sentence->green, sentence->blue, 1.0f);

	//폰트 쉐이더를 사용하여 텍스트를 렌더링
	result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(), pixelColor);

	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetFps(int fps, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char fpsString[16];
	float red, green, blue;
	bool result;
	
	//Fps를 10,000이하로 조정
	if (fps > 9999)
	{
		fps = 9999;
	}

	//Fps 정수를 문자열 형태로 변환
	_itoa_s(fps, tempString, 10);
	
	//fps 문자열 설정
	strcpy_s(fpsString, "Fps : ");
	strcat_s(fpsString, tempString);

	//만약 fps가 60 이상이라면 초록색
	if (fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	//만약 fps가 60 아래라면 노랑
	if (fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	//만약 fps가 30 아래라면 빨강
	if (fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	//문장 정점 버퍼에 새 문자열 정보를 업데이트
	result = UpdateSentence(m_sentence1, fpsString, 20, 20, red, green, blue, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetCpu(int cpu, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char cpuString[16];
	bool result;

	//Fps 정수를 문자열 형태로 변환
	_itoa_s(cpu, tempString, 10);

	//fps 문자열 설정
	strcpy_s(cpuString, "CPU : ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	//문장 정점 버퍼에 새 문자열 정보를 업데이트
	result = UpdateSentence(m_sentence2, cpuString, 20, 40, 0.0f,1.0f,0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}