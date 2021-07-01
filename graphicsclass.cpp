#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_MultiTextureShader = 0;
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

	//�� ��ü�� ����
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}
	
	//�� ��ü�� �ʱ�ȭ
	result = m_Model->Initialize(m_D3D->GetDevice(), (char*)"sphere.txt",(WCHAR*) L"stone01.dds",(WCHAR*) L"dirt01.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	//��Ƽ �ؽ�ó ���̴� ��ü�� ����
	m_MultiTextureShader = new MultiTextureShaderClass;
	if (!m_MultiTextureShader)
	{
		return false;
	}

	//��Ƽ�ؽ�ó ���̴� ��ü�� �ʱ�ȭ
	result = m_MultiTextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the multi texture shader object.", L"Error", MB_OK);
		return false;
	}


	return true;
}
void GraphicsClass::Shutdown()
{
	//��Ƽ �ؽ�ó ���̴� ��ü ��ȯ
	if (m_MultiTextureShader)
	{
		m_MultiTextureShader->Shutdown();
		delete m_MultiTextureShader;
		m_MultiTextureShader = 0;
	}
	
	//�� ��ü ��ȯ
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
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
bool GraphicsClass::Frame()
{
	bool result;

	//ī�޶� �������� ����
	m_Camera->SetPosition(0.0f,0.0f,-5.0f);

	return true;
}
bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix,orthoMatrix;

	//�� �׸��⸦ �����ϱ� ���� ������ ������ ����
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//ī�޶� ��ġ������ �� ����� ����
	m_Camera->Render();

	//ī�޶�� d3d������Ʈ�� ���� ����, ��, ��������� ������
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	//���� ������ �ε����� �׷��� ���������ο� ����
	m_Model->Render(m_D3D->GetDeviceContext());

	//
	m_MultiTextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTextureArray());

	//���ۿ� �׷��� ���� ȭ�鿡 ǥ��
	m_D3D->EndScene();
	
	return true;
}