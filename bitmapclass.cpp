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

	//스크린 사이즈와 비트맵 사이즈를를 저장
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_bitmapWidth = bitmapWidth*0.5;
	m_bitmapHeight = bitmapHeight*0.5;

	//렌더링 위치 변수를 -1로 초기화
	m_previousPosX = -1;
	m_previousPosY = -1;

	//정점 버퍼와 인덱스 버퍼를 초기화
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	//모델 텍스처를 초기화
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void BitmapClass::Shutdown()
{
	//모델 텍스처를 해제
	ReleaseTexture();

	//정점 버퍼와 인덱스 버퍼를 해제
	ShutdownBuffers();


	return;
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result;

	//이전 프레임과 위치가 바뀐경우 동적 정점 버퍼의 정점들을 새로운 위치로 갱신
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result)
	{
		return false;
	}

	//최종적으로 그릴 정점, 인덱스 버퍼를 준비
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

	//정점의 갯수 설정
	m_vertexCount = 6;

	//인덱스의 갯수 설정
	m_indexCount = 6;

	//정점 배열을 설정
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	//인덱스 배열을 설정
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

	//정점 버퍼의 description을 작성
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//정점 데이터를 가리키는 보조 리소스 구조체를 작성
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//정점 버퍼를 생성
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//인덱스 버퍼의 decription을 작성
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	
	//인덱스 데이터를 가리키는 보조 리소스 구조체를 작성
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//인덱스 버퍼를 생성
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	//생성되고 값이 할당된 정점 벞와 인덱스 버퍼를 해제
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;

	return true;
}

void BitmapClass::ShutdownBuffers()
{
	//인덱스 버퍼를 해제
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

	//만약 이미지의 위치가 이전과 같다면(이동이 없다면) 그냥 넘어감
	if ((positionX == m_previousPosX) && (positionX == m_previousPosY))
	{
		return true;
	}

	//만약 위치가 비뀌었다면
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	//좌측 변을 계산
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;
	//우측 변을 계산
	right = left + (float)m_bitmapWidth;
	//상단 변을 계산
	top = (float)((m_screenHeight) / 2) - (float)positionY;
	//하단 변을 계산
	bottom = top-(float)m_bitmapHeight;

	//정점 배열을 생성
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	//정점 배열에 데이터를 불러옴
	//첫 삼각형
	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);
	vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

	//두번째 삼각형
	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);
	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);
	vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);
	vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

	//수정 할 수 있게 정점 버퍼를 잠금
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//정점 버퍼 포인터를 저장
	verticesPtr = (VertexType*)mappedResource.pData;

	//정점 버퍼에 데이터를 저장
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	//정점 버퍼 잠금 해제
	deviceContext->Unmap(m_vertexBuffer, 0);

	//정점 배열이 더 이상 필요가 없으므로 해제
	delete[] vertices;
	vertices = 0;
	
	return true;

}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	//정점 버퍼의 단위와 오프셋을 설정
	stride = sizeof(VertexType);
	offset = 0;

	//그릴 수 있도록 input asssmbler에 정점 버퍼를 활성화
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//input assembler에 인덱스 버퍼를 활성화
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//정점 버퍼로 그릴 기본형을 설정
	//삼각형
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool BitmapClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	bool result;

	//텍스처 오브젝트 생성
	m_texture = new TextureClass;
	if (!m_texture)
	{
		return false;
	}

	//텍스처 오브젝트 초기화
	result = m_texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}
	return true;
}
void BitmapClass::ReleaseTexture()
{
	//텍스처 오브젝트를 해제
	if (m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}
	return;
}
