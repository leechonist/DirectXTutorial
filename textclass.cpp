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

	//ȭ�� ����, ���� ����
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	//�⺻ �� ��� ����
	m_baseViewMatrix = baseViewMatrix;

	//��Ʈ ��ü ����
	m_Font = new FontClass;
	if (!m_Font)
	{
		return false;
	}

	//��Ʈ ��ü �ʱ�ȭ
	result = m_Font->Initialize(device, (char*)"fontdata.txt", L"font.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	//��Ʈ���̴� ��ü ����
	m_FontShader = new FontShaderClass;
	if (!m_FontShader)
	{
		return false;
	}

	//��Ʈ ���̴��� ����
	result = m_FontShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initailize the font shader object.", L"Error", MB_OK);
		return false;
	}

	//ù��° ������ �ʱ�ȭ
	result = InitializeSentence(&m_sentence1, 16, device);
	if (!result)
	{
		return false;
	}

	//���� ���� ���۸� �� ������ �Բ� �ʱ�ȭ
	result = UpdateSentence(m_sentence1, (char*)"Hello", 100, 100, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	//�� ��° ������ �ʱ�ȭ
	result = InitializeSentence(&m_sentence2, 16, device);
	if (!result)
	{
		return false;
	}

	//���� ���� ���۸� �� ������ �Բ� �ʱ�ȭ
	result = UpdateSentence(m_sentence2, (char*)"Goodbye", 100, 200, 1.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

void TextClass::Shutdown()
{
	//ù��° ���� ����
	ReleaseSentence(&m_sentence1);

	//�� ��° ���� ����
	ReleaseSentence(&m_sentence2);

	//��Ʈ ���̴� ��ü ����
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	//��Ʈ ��ü ����
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

	//ù��° ������ �׸�
	result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	//�� ��° ������ �׸�
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

	//���ο� ���� ��ü�� ����
	*sentence = new SentenceType;
	if (!*sentence)
	{
		return false;
	}

	//���� ���۸� null�� �ʱ�ȭ
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	//������ �ִ� ���̸� ����
	(*sentence)->maxLength = maxLength;

	//���� �迭�� ������ ����
	(*sentence)->vertexCount = 6 * maxLength;
	
	//�ε��� �迭�� ������ ����
	(*sentence)->indexCount = (*sentence)->vertexCount;

	//���� �迭 ����
	vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}
	
	//�ε��� �迭 ����
	indices = new unsigned long[(*sentence)->indexCount];
	if (!indices)
	{
		return false;
	}

	//���� �迭�� 0���� �ʱ�ȭ
	memset(vertices, 0, (sizeof(VertexType) * ((*sentence)->vertexCount)));

	//�ε��� �迭�� �ʱ�ȭ
	for (i = 0; i < (*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	//���� ���� ���۸� ����
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

	//���� ���� ����
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//���� �ε��� ���� ��ũ���� ����
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

	//�ε��� ���� ����
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//���� �迭�� ����
	delete [] vertices;
	vertices = 0;

	//�ε��� �迭�� ����
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

	//������ ���� ����
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	//������ ���̸� ����
	numLetters = (int)strlen(text);

	//�����÷ο찡 �߻��ߴ��� üũ
	if (numLetters > sentence->maxLength)
	{
		return false;
	}

	//���� �迭�� ����
	vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	//���� �迭�� 0���� �ʱ�ȭ
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	//�׷��� X,Y ��ġ�� ���
	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);

	//��Ʈ Ŭ������ ����� ���� �迭�� ����
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	//���� ���۸� ���
	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//���� ���� ������ ����
	verticesPtr = (VertexType*)mappedResource.pData;

	//�����͸� ���� ���ۿ� ����
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	//�������� ��� ����
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	//���� �迭 ����
	delete[] vertices;
	vertices = 0;

	return true;
}

void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence)
	{
		//���� ���۸� ����
		if ((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		//�ε��� ���۸� ����
		if ((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		//������ ����
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

	//���� ������ stride�� offset�� ����
	stride = sizeof(VertexType);
	offset = 0;

	//���� ���۸� ������ �� �� �ֵ��� ������� �־� Ȱ��ȭ
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	//�ε��� ���۸� ������ �� �� �ֵ��� ������� �־� Ȱ��ȭ
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//���� ���۰� �׷��� ������ ���� ���⼭�� �ﰢ��
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//�ȼ� ���� ���͸� ����
	pixelColor = D3DXVECTOR4(sentence->red, sentence->green, sentence->blue, 1.0f);

	//��Ʈ ���̴��� ����Ͽ� �ؽ�Ʈ�� ������
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
	
	//Fps�� 10,000���Ϸ� ����
	if (fps > 9999)
	{
		fps = 9999;
	}

	//Fps ������ ���ڿ� ���·� ��ȯ
	_itoa_s(fps, tempString, 10);
	
	//fps ���ڿ� ����
	strcpy_s(fpsString, "Fps : ");
	strcat_s(fpsString, tempString);

	//���� fps�� 60 �̻��̶�� �ʷϻ�
	if (fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	//���� fps�� 60 �Ʒ���� ���
	if (fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	//���� fps�� 30 �Ʒ���� ����
	if (fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	//���� ���� ���ۿ� �� ���ڿ� ������ ������Ʈ
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

	//Fps ������ ���ڿ� ���·� ��ȯ
	_itoa_s(cpu, tempString, 10);

	//fps ���ڿ� ����
	strcpy_s(cpuString, "CPU : ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	//���� ���� ���ۿ� �� ���ڿ� ������ ������Ʈ
	result = UpdateSentence(m_sentence2, cpuString, 20, 40, 0.0f,1.0f,0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}