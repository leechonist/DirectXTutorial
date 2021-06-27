#include "fontshaderclass.h"

FontShaderClass::FontShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_constantBuffer = 0;
	m_sampleState = 0;
	m_pixelBuffer = 0;
}
FontShaderClass::FontShaderClass(const FontShaderClass& other)
{
}
FontShaderClass::~FontShaderClass()
{
}

bool FontShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	//���� �ȼ� ���̴��� �ʱ�ȭ
	result = InitializeShader(device, hwnd, L"Font.vs", L"Font.ps");
	if (!result)
	{
		return false;
	}

	return true;
}
void FontShaderClass::Shutdown()
{
	//����, �ȼ� ���̴��� ���õ� ������Ʈ���� ����
	ShutdownShader();

	return;
}

bool FontShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture,D3DXVECTOR4 pixelColor)
{
	bool result;
	//�������� ���� ���̴� �Ķ���͸� ����
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, pixelColor);
	if (!result)
	{
		return false;
	}

	//���� �غ�� ���۸� ���̴��� ������
	RenderShader(deviceContext, indexCount);

	return true;
}
bool FontShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC constantBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC pixelBufferDesc;

	//NULL�� ����ϱ� ���� �����͵��� �ʱ�ȭ
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//���� ���̴� �ڵ带 ������
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "FontVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		//�����Ͽ� ������ ��� ������ errorMessage�� ������ �ۼ�
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		//���� errorMessage�� �ƹ� ������ ���ٸ� ���̴� ������ ã�� �� ���� �����
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing ShaderFilename", MB_OK);
		}
		return false;
	}

	//�ȼ� ���̴� �ڵ带 ������
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "FontPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		//�����Ͽ� ������ ��� ������ errorMessage�� ������ �ۼ�
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		//���� errorMessage�� �ƹ� ������ ���ٸ� ���̴� ������ ã�� �� ���� �����
		else
		{
			MessageBox(hwnd, psFilename, L"Missing ShaderFilename", MB_OK);
		}
		return false;
	}

	//���ۿ� ���� ���̴��� ����
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"VertexShader", L"Problem", MB_OK);
		return false;
	}

	//���ۿ� �ȼ� ���̴��� ����
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"PixelShader", L"Problem", MB_OK);
		return false;
	}

	//���� �Է� �����̿� description�� ����
	//�� ������ ModelClass�� ���̴��� �ִ� VertexType����ü�� ��ġ�ؾ���

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//���̾ƿ��� ���� ������ ����
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//���� �Է� ���̾ƿ��� ����
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Layout", L"Probelm", MB_OK);
		return false;
	}

	//�� �̻� ���� ���̴� ���ۿ� �ȼ� ���̴� ���۴� �ʿ� ���� ������ ����
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;


	//????
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(ConstantBufferType);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	//??
	result = device->CreateBuffer(&constantBufferDesc, NULL, &m_constantBuffer);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"MatrixBuffer", L"Probelm", MB_OK);
		return false;
	}

	//�ؽ�ó ���÷� ���� description ����

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//�ؽ�ó ���÷� ���¸� ����
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Sampler", L"Probelm", MB_OK);
		return false;
	}

	//�弿 ���̴��� ���� ���� ������ ������۸� ����
	pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
	pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelBufferDesc.MiscFlags = 0;
	pixelBufferDesc.StructureByteStride = 0;

	//��� ���� �����͸� ����
	result = device->CreateBuffer(&pixelBufferDesc, NULL, &m_pixelBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void FontShaderClass::ShutdownShader()
{
	//�ȼ� ��� ���۸� ����
	if (m_pixelBuffer)
	{
		m_pixelBuffer->Release();
		m_pixelBuffer = 0;
	}

	//���÷� ���¸� ����
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	//��� ���� ����� ����
	if (m_constantBuffer)
	{
		m_constantBuffer->Release();
		m_constantBuffer = 0;
	}

	//���̾ƿ��� ����
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	//�ȼ� ���̴��� ����
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	//���� ���̴��� ����
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

void FontShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	//�����޽��� �ؽ�ó ���ۿ��� �����͸� ����
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//�޽����� ���̸� ����
	bufferSize = errorMessage->GetBufferSize();

	//�޼����� �����ϱ� ���� ���� ����/����
	fout.open("shader-error.txt");

	//�����޽��� �ۼ�
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	//���� ����
	fout.close();

	//�����޽��� ����
	errorMessage->Release();
	errorMessage = 0;

	//�޽��� �˾�
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.text for message.", shaderFilename, MB_OK);

	return;
}

bool FontShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR4 pixelColor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ConstantBufferType* dataPtr;
	unsigned int bufferNumber;
	PixelBufferType* dataPtr2;

	//������۸� ���
	result = deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//������� �����͸� ����
	dataPtr = (ConstantBufferType*)mappedResource.pData;

	//���̴��� ���� ����� �̵�
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	//��� ���� �ȿ� ��ĵ��� ����
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	//������� ��� ����
	deviceContext->Unmap(m_constantBuffer, 0);

	//���� ���̴����� ��������� ��ġ�� ����
	bufferNumber = 0;

	//������Ʈ�� ������۸� �������̴��� ����
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_constantBuffer);

	//�ȼ� ���̴��� �ؽ�ó���ҽ��� ����
	deviceContext->PSSetShaderResources(0, 1, &texture);

	//�ȼ� ��� ���۸� ���
	result = deviceContext->Map(m_pixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr2 = (PixelBufferType*)mappedResource.pData;

	//�ȼ� ������ �ȼ� ��� ���ۿ� ����
	dataPtr2->pixelColor = pixelColor;

	//�ȼ� ��� ���۸� ��� ����
	deviceContext->Unmap(m_pixelBuffer, 0);

	bufferNumber = 0;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pixelBuffer);

	return true;
}

void FontShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//�Է� ���̾ƿ��� ����
	deviceContext->IASetInputLayout(m_layout);

	//�ﰢ���� �������� ����, �ȼ� ���̴��� ����
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//�ȼ� ���̴��� ���÷� ���¸� ����
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	//�ﰢ���� �׸�
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
