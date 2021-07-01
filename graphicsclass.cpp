#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightMapShader = 0;
}
GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}
GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	D3DXMATRIX baseViewMatrix;

	//Direct3D 객체를 생성
	m_D3D = new D3DClass;
	if (!m_D3D)
	{
		return false;
	}

	//Direct3D 객체를 초기화
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct 3D", L"Error", MB_OK);
		return false;
	}

	//카메라 객체를 생성
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}
	//카메라 객체의 위치를 설정
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	//모델 객체를 생성
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}
	
	//모델 객체를 초기화
	result = m_Model->Initialize(m_D3D->GetDevice(), (char*)"sphere.txt",(WCHAR*) L"stone01.dds",(WCHAR*) L"light01.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	//멀티 텍스처 쉐이더 객체를 생성
	m_LightMapShader = new LightMapShaderClass;
	if (!m_LightMapShader)
	{
		return false;
	}

	//멀티텍스처 쉐이더 객체를 초기화
	result = m_LightMapShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the multi texture shader object.", L"Error", MB_OK);
		return false;
	}


	return true;
}
void GraphicsClass::Shutdown()
{
	//멀티 텍스처 쉐이더 객체 반환
	if (m_LightMapShader)
	{
		m_LightMapShader->Shutdown();
		delete m_LightMapShader;
		m_LightMapShader = 0;
	}
	
	//모델 객체 반환
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	//Camera객체를 반환
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	//D3D 객체를 반환
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
	return;
}
bool GraphicsClass::Frame()
{
	bool result;

	//카메라 포지션을 설정
	m_Camera->SetPosition(0.0f,0.0f,-5.0f);

	return true;
}
bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix,orthoMatrix;

	//씬 그리기를 시작하기 위해 버퍼의 내용을 지움
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//카메라 위치에서의 뷰 행렬을 생성
	m_Camera->Render();

	//카메라와 d3d오브젝트를 통해 월드, 뷰, 투영행렬을 가져옴
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	//모델의 정점과 인덱스를 그래픽 파이프라인에 전달
	m_Model->Render(m_D3D->GetDeviceContext());

	//
	m_LightMapShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTextureArray());

	//버퍼에 그려진 씬을 화면에 표시
	m_D3D->EndScene();
	
	return true;
}