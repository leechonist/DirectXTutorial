#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Text = 0;
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

	//텍스트 객체를 생성
	m_Text = new TextClass;
	if (!m_Text)
	{
		return false;
	}

	//텍스트 객체를 초기화
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(),hwnd,screenWidth,screenHeight,baseViewMatrix);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
	}

	return true;
}
void GraphicsClass::Shutdown()
{
	//textureShader 객체를 반환
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
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
bool GraphicsClass::Frame(int mouseX,int mouseY)
{
	bool result;

	//마우스 위치를 설정
	result = m_Text->SetMousePosition(mouseX, mouseY, m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}
	
	//카메라 포지션을 설정
	m_Camera->SetPosition(0.0f,0.0f,-10.0f);

	return true;
}
bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix,orthoMatrix;
	bool result;
	//씬 그리기를 시작하기 위해 버퍼의 내용을 지움
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//카메라 위치에서의 뷰 행렬을 생성
	m_Camera->Render();

	//카메라와 d3d오브젝트를 통해 월드, 뷰, 투영행렬을 가져옴
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	//2D 렌더링을 시작하기 위해 Z Buffer를 꺼줌
	m_D3D->TurnZBufferOff();

	//텍스트를 렌더링 하기 위해 알파 블렌딩을 켬
	m_D3D->TurnOnAlphaBlending();

	//텍스트를 출력
	result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	//텍스트를 모두 출력했으면 알파 블렌딩 종료
	m_D3D->TurnOffAlphaBlending();

	//2D렌더링이 끝났으면 Z Buffer를 켜줌
	m_D3D->TurnZBufferOn();

	//버퍼에 그려진 씬을 화면에 표시
	m_D3D->EndScene();
	
	return true;
}