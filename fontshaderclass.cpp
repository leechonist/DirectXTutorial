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
	//정점 픽셀 쉐이더를 초기화
	result = InitializeShader(device, hwnd, L"Font.vs", L"Font.ps");
	if (!result)
	{
		return false;
	}

	return true;
}
void FontShaderClass::Shutdown()
{
	//정점, 픽셀 쉐이더와 관련된 오브젝트들을 해제
	ShutdownShader();

	return;
}

bool FontShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture,D3DXVECTOR4 pixelColor)
{
	bool result;
	//렌더링에 사용될 쉐이더 파라미터를 설정
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, pixelColor);
	if (!result)
	{
		return false;
	}

	//이제 준비된 버퍼를 쉐이더와 렌더링
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

	//NULL로 사용하기 위해 포인터들을 초기화
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//정점 쉐이더 코드를 컴파일
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "FontVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		//컴파일에 실패할 경우 내용을 errorMessage에 내용을 작성
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		//만약 errorMessage에 아무 내용이 없다면 쉐이더 파일을 찾을 수 없는 경우임
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing ShaderFilename", MB_OK);
		}
		return false;
	}

	//픽셀 쉐이더 코드를 컴파일
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "FontPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		//컴파일에 실패할 경우 내용을 errorMessage에 내용을 작성
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		//만약 errorMessage에 아무 내용이 없다면 쉐이더 파일을 찾을 수 없는 경우임
		else
		{
			MessageBox(hwnd, psFilename, L"Missing ShaderFilename", MB_OK);
		}
		return false;
	}

	//버퍼에 정점 쉐이더를 생성
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"VertexShader", L"Problem", MB_OK);
		return false;
	}

	//버퍼에 픽셀 쉐이더를 생성
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"PixelShader", L"Problem", MB_OK);
		return false;
	}

	//정점 입력 레아이웃 description을 생성
	//이 설정은 ModelClass와 쉐이더에 있는 VertexType구조체와 일치해야함

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

	//레이아웃의 원소 개수를 저장
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//정점 입력 레이아웃을 생성
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Layout", L"Probelm", MB_OK);
		return false;
	}

	//더 이상 정점 쉐이더 버퍼와 픽셀 쉐이더 버퍼는 필요 없기 때문에 해제
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

	//텍스처 샘플러 상태 description 생성

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

	//텍스처 샘플러 상태를 생성
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Sampler", L"Probelm", MB_OK);
		return false;
	}

	//펙셀 쉐이더에 글자 색을 지정할 상수버퍼를 생성
	pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
	pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelBufferDesc.MiscFlags = 0;
	pixelBufferDesc.StructureByteStride = 0;

	//상수 버퍼 포인터를 생성
	result = device->CreateBuffer(&pixelBufferDesc, NULL, &m_pixelBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void FontShaderClass::ShutdownShader()
{
	//픽셀 상수 버퍼를 해제
	if (m_pixelBuffer)
	{
		m_pixelBuffer->Release();
		m_pixelBuffer = 0;
	}

	//샘플러 상태를 해제
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	//상수 버퍼 행렬을 해제
	if (m_constantBuffer)
	{
		m_constantBuffer->Release();
		m_constantBuffer = 0;
	}

	//레이아웃을 해제
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	//픽셀 쉐이더를 해제
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	//정점 쉐이더를 해제
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

	//에러메시지 텍스처 버퍼에서 포인터를 저장
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//메시지의 길이를 저장
	bufferSize = errorMessage->GetBufferSize();

	//메세지를 저장하기 위한 파일 생성/열기
	fout.open("shader-error.txt");

	//에러메시지 작성
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	//파일 종료
	fout.close();

	//에러메시지 해제
	errorMessage->Release();
	errorMessage = 0;

	//메시지 팝업
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

	//상수버퍼를 잠금
	result = deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//상수버퍼 포인터를 저장
	dataPtr = (ConstantBufferType*)mappedResource.pData;

	//쉐이더를 위해 행렬을 이동
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	//사수 버퍼 안에 행렬들을 저장
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	//상수버퍼 잠금 해제
	deviceContext->Unmap(m_constantBuffer, 0);

	//정점 쉐이더에서 상수버퍼의 위치를 저장
	bufferNumber = 0;

	//업데이트된 상수버퍼를 정점쉐이더에 저장
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_constantBuffer);

	//픽셀 쉐이더에 텍스처리소스를 저장
	deviceContext->PSSetShaderResources(0, 1, &texture);

	//픽셀 상수 버퍼를 잠금
	result = deviceContext->Map(m_pixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr2 = (PixelBufferType*)mappedResource.pData;

	//픽셀 색상을 픽셀 상수 버퍼에 복사
	dataPtr2->pixelColor = pixelColor;

	//픽셀 상수 버퍼를 잠금 해제
	deviceContext->Unmap(m_pixelBuffer, 0);

	bufferNumber = 0;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pixelBuffer);

	return true;
}

void FontShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//입력 레이아웃을 설정
	deviceContext->IASetInputLayout(m_layout);

	//삼각형을 렌더링할 정점, 픽셀 쉐이더를 설정
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//픽셀 쉐이더에 샘플러 상태를 설정
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	//삼각형을 그림
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
