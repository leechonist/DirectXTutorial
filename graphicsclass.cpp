#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Text = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;
	m_ModelList = 0;
	m_Frustum = 0;
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

	//�� ��ü�� ����
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}
	
	//�� ��ü�� �ʱ�ȭ
	result = m_Model->Initialize(m_D3D->GetDevice(), (char*)"sphere.txt",L"seafloor.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	//���� ���̴� ��ü�� ����
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}

	//���� ���̴� ��ü�� �ʱ�ȭ
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	//���� ��ü�� �ʱ�ȭ
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}
	
	//���� ��ü�� �ʱ�ȭ
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(100.0f);
	//�� ����Ʈ ��ü�� ����
	m_ModelList = new ModelListClass;
	if (!m_ModelList)
	{
		return false;
	}

	//�� ����Ʈ ��ü�� �ʱ�ȭ
	result = m_ModelList->Initialize(25);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model list object.", L"Error", MB_OK);
		return false;
	}
	
	//ǻ������ ������Ʈ ����
	m_Frustum = new FrustumClass;
	if (!m_Frustum)
	{
		return false;
	}

	return true;
}
void GraphicsClass::Shutdown()
{
	//ǻ������ ��ü ��ȯ
	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	//�� ����Ʈ ��ü ��ȯ
	if (m_ModelList)
	{
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = 0;
	}
	
	//���� ��ü ��ȯ
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	//���� ���̴� ��ü ��ȯ
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	//�� ��ü ��ȯ
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	//�ؽ�Ʈ ��ü�� ����
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
bool GraphicsClass::Frame(float rotationY)
{
	bool result;

	//ī�޶� �������� ����
	m_Camera->SetPosition(0.0f,0.0f,-10.0f);

	//ī�޶� ȸ���� ����
	m_Camera->SetRotation(0.0f, rotationY, 0.0f);

	return true;
}
bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix,orthoMatrix;
	int modelCount, renderCount, index;
	float positionX, positionY, positionZ, radius;
	D3DXVECTOR4 color;
	bool renderModel, result;

	//�� �׸��⸦ �����ϱ� ���� ������ ������ ����
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//ī�޶� ��ġ������ �� ����� ����
	m_Camera->Render();

	//ī�޶�� d3d������Ʈ�� ���� ����, ��, ��������� ������
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	//ǻ������ ����
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	//������ �� ���� ���� ����
	modelCount = m_ModelList->GetModelCount();

	//���� ������ �� ���� ���� ������ �ʱ�ȭ
	renderCount = 0;
	
	//��� �𵨵��� �ѷ����� ī�޶� �信 �ִ� �͸� ������
	for (index = 0; index < modelCount; index++)
	{
		//�� ���� ��ġ�� �÷��� ����
		m_ModelList->GetData(index, positionX, positionY, positionZ, color);

		//�������� 1�� ����
		radius = 1.0f;

		//�� ���� �� �������� �ȿ� �ִ��� Ȯ��
		renderModel = m_Frustum->CheckSphere(positionX, positionY, positionZ, radius);

		//���� ���� �ȿ� �ִٸ� ������ �ϰ�, ���ٸ� ���� �𵨷� �Ѿ
		if (renderModel)
		{
			//���� ������ �� ��ġ�� �̵�
			D3DXMatrixTranslation(&worldMatrix, positionX, positionY, positionZ);
			
			//���� �׸��� ���� �׷��� ���������ο� ���� ����, �ε��� ���۸� ��
			m_Model->Render(m_D3D->GetDeviceContext());

			//���� ���̴��� �̿��� ���� ������
			m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				m_Model->GetTexture(), m_Light->GetDirection(), color);
			
			//���� ����� �ʱ�ȭ
			m_D3D->GetWorldMatrix(worldMatrix);

			//�� ���� ������ �Ǿ����� ī��Ʈ�� �߰���
			renderCount++;
		}
	}
	//�� ���� ���� ������ �Ǵ��� ����
	result = m_Text->SetRenderCount(renderCount, m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

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