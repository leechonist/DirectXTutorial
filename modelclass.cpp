#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_texture = 0;
	m_model = 0;
}
ModelClass::ModelClass(const ModelClass& other)
{
}
ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device,char* modelFilename, const WCHAR* textureFilename)
{
	bool result;

	//모델데이터를 불러옴
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}
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

void ModelClass::Shutdown()
{
	//모델 텍스처를 해제
	ReleaseTexture();

	//정점 버퍼와 인덱스 버퍼를 해제
	ShutdownBuffers();

	//모델 데이터를 해제
	ReleaseModel();

	return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	//정점 버퍼와 인덱스 버퍼를 그래픽스 파이프라인에 넣어 화면에 그릴 준비
	RenderBuffers(deviceContext);

	return;
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

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

	//정점과 인덱스 배열을 불러옵니다
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

	//정점 버퍼의 description을 작성
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
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

void ModelClass::ShutdownBuffers()
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

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

bool ModelClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
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
void ModelClass::ReleaseTexture()
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

bool ModelClass::LoadModel(char* filename)
{
	ifstream fin;
	char input;
	int i;

	//모델 파일 열기
	fin.open(filename);

	//만약 파일이 열리지 않았다면 종료
	if (fin.fail())
	{
		return false;
	}

	//정점 갯수가 나올 때 까지 읽어옴
	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}

	//정점 갯수를 읽어옴
	fin >> m_vertexCount;

	//인덱스 갯수를 정점 갯수와 맞춤
	m_indexCount = m_vertexCount;
	
	//읽기 위한 모델을 생성(?)
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	//데이터 시작 전 까지 읽어옴
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	//정점 데이터를 읽어옴
	for (i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	//모델 파일을 종료
	fin.close();
}

void ModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}
	return;
}