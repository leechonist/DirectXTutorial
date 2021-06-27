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

	//Direct3D ��ü�� ����
	m_D3D = new D3DClass;
	if (!m_D3D)
	{
		return false;
	}

	//Direct3D ��ü�� �ʱ�ȭ
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct 3D", L"Error", MB_OK);
		return false;
	}

	//ī�޶� ��ü�� ����
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}
	//ī�޶� ��ü�� ��ġ�� ����
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	//�ؽ�Ʈ ��ü�� ����
	m_Text = new TextClass;
	if (!m_Text)
	{
		return false;
	}

	//�ؽ�Ʈ ��ü�� �ʱ�ȭ
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(),hwnd,screenWidth,screenHeight,baseViewMatrix);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
	}

	return true;
}
void GraphicsClass::Shutdown()
{
	//textureShader ��ü�� ��ȯ
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	//Camera��ü�� ��ȯ
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	//D3D ��ü�� ��ȯ
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

	//���콺 ��ġ�� ����
	result = m_Text->SetMousePosition(mouseX, mouseY, m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}
	
	//ī�޶� �������� ����
	m_Camera->SetPosition(0.0f,0.0f,-10.0f);

	return true;
}
bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix,orthoMatrix;
	bool result;
	//�� �׸��⸦ �����ϱ� ���� ������ ������ ����
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//ī�޶� ��ġ������ �� ����� ����
	m_Camera->Render();

	//ī�޶�� d3d������Ʈ�� ���� ����, ��, ��������� ������
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	//2D �������� �����ϱ� ���� Z Buffer�� ����
	m_D3D->TurnZBufferOff();

	//�ؽ�Ʈ�� ������ �ϱ� ���� ���� ������ ��
	m_D3D->TurnOnAlphaBlending();

	//�ؽ�Ʈ�� ���
	result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	//�ؽ�Ʈ�� ��� ��������� ���� ���� ����
	m_D3D->TurnOffAlphaBlending();

	//2D�������� �������� Z Buffer�� ����
	m_D3D->TurnZBufferOn();

	//���ۿ� �׷��� ���� ȭ�鿡 ǥ��
	m_D3D->EndScene();
	
	return true;
}