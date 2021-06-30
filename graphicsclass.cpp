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

	//모델 객체를 생성
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}
	
	//모델 객체를 초기화
	result = m_Model->Initialize(m_D3D->GetDevice(), (char*)"sphere.txt",L"seafloor.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	//광원 쉐이더 객체를 생성
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}

	//광원 쉐이더 객체를 초기화
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	//광원 객체를 초기화
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}
	
	//광원 객체를 초기화
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(100.0f);
	//모델 리스트 객체를 생성
	m_ModelList = new ModelListClass;
	if (!m_ModelList)
	{
		return false;
	}

	//모델 리스트 객체를 초기화
	result = m_ModelList->Initialize(25);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model list object.", L"Error", MB_OK);
		return false;
	}
	
	//퓨러스텀 오브젝트 생성
	m_Frustum = new FrustumClass;
	if (!m_Frustum)
	{
		return false;
	}

	return true;
}
void GraphicsClass::Shutdown()
{
	//퓨러스텀 객체 반환
	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	//모델 리스트 객체 반환
	if (m_ModelList)
	{
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = 0;
	}
	
	//광원 객체 반환
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	//광원 쉐이더 객체 반환
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	//모델 객체 반환
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	//텍스트 객체를 해제
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
bool GraphicsClass::Frame(float rotationY)
{
	bool result;

	//카메라 포지션을 설정
	m_Camera->SetPosition(0.0f,0.0f,-10.0f);

	//카메라 회전을 설정
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

	//씬 그리기를 시작하기 위해 버퍼의 내용을 지움
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//카메라 위치에서의 뷰 행렬을 생성
	m_Camera->Render();

	//카메라와 d3d오브젝트를 통해 월드, 뷰, 투영행렬을 가져옴
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	//퓨러스텀 구축
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	//렌더링 될 모델의 수를 저장
	modelCount = m_ModelList->GetModelCount();

	//모델이 렌더링 된 수를 세는 변수를 초기화
	renderCount = 0;
	
	//모든 모델들을 둘러보고 카메라 뷰에 있는 것만 렌더링
	for (index = 0; index < modelCount; index++)
	{
		//구 모델의 위치와 컬러를 저장
		m_ModelList->GetData(index, positionX, positionY, positionZ, color);

		//반지름을 1로 설정
		radius = 1.0f;

		//구 모델이 뷰 프러스텀 안에 있는지 확인
		renderModel = m_Frustum->CheckSphere(positionX, positionY, positionZ, radius);

		//만약 모델이 안에 있다면 렌더링 하고, 없다면 다음 모델로 넘어감
		if (renderModel)
		{
			//모델을 렌더링 될 위치로 이동
			D3DXMatrixTranslation(&worldMatrix, positionX, positionY, positionZ);
			
			//모델을 그리기 위해 그래픽 파이프라인에 모델의 정점, 인덱스 버퍼를 둠
			m_Model->Render(m_D3D->GetDeviceContext());

			//광원 쉐이더를 이용해 모델을 렌더링
			m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				m_Model->GetTexture(), m_Light->GetDirection(), color);
			
			//월드 행렬을 초기화
			m_D3D->GetWorldMatrix(worldMatrix);

			//이 모델이 렌더링 되었으면 카운트를 추가함
			renderCount++;
		}
	}
	//몇 개의 모델이 렌더링 되는지 저장
	result = m_Text->SetRenderCount(renderCount, m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

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