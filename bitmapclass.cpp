#include "bitmapclass.h"

BitmapClass::BitmapClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_texture = 0;
}
BitmapClass::BitmapClass(const BitmapClass& other)
{
}
BitmapClass::~BitmapClass()
{
}

bool BitmapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, const WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	bool result;

	//��ũ�� ������� ��Ʈ�� ������� ����
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_bitmapWidth = bitmapWidth*0.5;
	m_bitmapHeight = bitmapHeight*0.5;

	//������ ��ġ ������ -1�� �ʱ�ȭ
	m_previousPosX = -1;
	m_previousPosY = -1;

	//���� ���ۿ� �ε��� ���۸� �ʱ�ȭ
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	//�� �ؽ�ó�� �ʱ�ȭ
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void BitmapClass::Shutdown()
{
	//�� �ؽ�ó�� ����
	ReleaseTexture();

	//���� ���ۿ� �ε��� ���۸� ����
	ShutdownBuffers();


	return;
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result;

	//���� �����Ӱ� ��ġ�� �ٲ��� ���� ���� ������ �������� ���ο� ��ġ�� ����
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result)
	{
		return false;
	}

	//���������� �׸� ����, �ε��� ���۸� �غ�
	RenderBuffers(deviceContext);

	return true;
}

int BitmapClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return m_texture->GetTexture();
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	//������ ���� ����
	m_vertexCount = 6;

	//�ε����� ���� ����
	m_indexCount = 6;

	//���� �迭�� ����
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	//�ε��� �迭�� ����
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	memset(vertices, 0, sizeof(VertexType) * m_vertexCount);
	for (int i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	//���� ������ description�� �ۼ�
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//���� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ�
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//���� ���۸� ����
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//�ε��� ������ decription�� �ۼ�
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	
	//�ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ�
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//�ε��� ���۸� ����
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	//�����ǰ� ���� �Ҵ�� ���� ���� �ε��� ���۸� ����
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;

	return true;
}

void BitmapClass::ShutdownBuffers()
{
	//�ε��� ���۸� ����
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
	return;
}

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;

	//���� �̹����� ��ġ�� ������ ���ٸ�(�̵��� ���ٸ�) �׳� �Ѿ
	if ((positionX == m_previousPosX) && (positionX == m_previousPosY))
	{
		return true;
	}

	//���� ��ġ�� �����ٸ�
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	//���� ���� ���
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;
	//���� ���� ���
	right = left + (float)m_bitmapWidth;
	//��� ���� ���
	top = (float)((m_screenHeight) / 2) - (float)positionY;
	//�ϴ� ���� ���
	bottom = top-(float)m_bitmapHeight;

	//���� �迭�� ����
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	//���� �迭�� �����͸� �ҷ���
	//ù �ﰢ��
	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);
	vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

	//�ι�° �ﰢ��
	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);
	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);
	vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);
	vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

	//���� �� �� �ְ� ���� ���۸� ���
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//���� ���� �����͸� ����
	verticesPtr = (VertexType*)mappedResource.pData;

	//���� ���ۿ� �����͸� ����
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	//���� ���� ��� ����
	deviceContext->Unmap(m_vertexBuffer, 0);

	//���� �迭�� �� �̻� �ʿ䰡 �����Ƿ� ����
	delete[] vertices;
	vertices = 0;
	
	return true;

}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	//���� ������ ������ �������� ����
	stride = sizeof(VertexType);
	offset = 0;

	//�׸� �� �ֵ��� input asssmbler�� ���� ���۸� Ȱ��ȭ
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//input assembler�� �ε��� ���۸� Ȱ��ȭ
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//���� ���۷� �׸� �⺻���� ����
	//�ﰢ��
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool BitmapClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	bool result;

	//�ؽ�ó ������Ʈ ����
	m_texture = new TextureClass;
	if (!m_texture)
	{
		return false;
	}

	//�ؽ�ó ������Ʈ �ʱ�ȭ
	result = m_texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}
	return true;
}
void BitmapClass::ReleaseTexture()
{
	//�ؽ�ó ������Ʈ�� ����
	if (m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}
	return;
}
