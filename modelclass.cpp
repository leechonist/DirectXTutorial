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

	//�𵨵����͸� �ҷ���
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}
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

void ModelClass::Shutdown()
{
	//�� �ؽ�ó�� ����
	ReleaseTexture();

	//���� ���ۿ� �ε��� ���۸� ����
	ShutdownBuffers();

	//�� �����͸� ����
	ReleaseModel();

	return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	//���� ���ۿ� �ε��� ���۸� �׷��Ƚ� ���������ο� �־� ȭ�鿡 �׸� �غ�
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

	//������ �ε��� �迭�� �ҷ��ɴϴ�
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

	//���� ������ description�� �ۼ�
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
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

void ModelClass::ShutdownBuffers()
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

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

bool ModelClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
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
void ModelClass::ReleaseTexture()
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

bool ModelClass::LoadModel(char* filename)
{
	ifstream fin;
	char input;
	int i;

	//�� ���� ����
	fin.open(filename);

	//���� ������ ������ �ʾҴٸ� ����
	if (fin.fail())
	{
		return false;
	}

	//���� ������ ���� �� ���� �о��
	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}

	//���� ������ �о��
	fin >> m_vertexCount;

	//�ε��� ������ ���� ������ ����
	m_indexCount = m_vertexCount;
	
	//�б� ���� ���� ����(?)
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	//������ ���� �� ���� �о��
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	//���� �����͸� �о��
	for (i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	//�� ������ ����
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