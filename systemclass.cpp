#include "systemclass.h"

SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
}
SystemClass::SystemClass(const SystemClass& other)
{
}
SystemClass::~SystemClass()
{
}
bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	//함수에 높이와 너비를 전달하기 전에 변수를 0으로 초기화
	screenWidth = screenHeight = 0;

	InitializeWindows(screenWidth, screenHeight);

	//input 객체를 생성 키보드 입력을 처리하기 위해 사용
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}
	
	//Input 객체를 초기화
	result = m_Input->Initialize(m_hinstance,m_hwnd,screenWidth,screenHeight);
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	//graphics 객체를 생성 그래픽 요소를 그리기 위해 사용
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	//graphics 객체를 초기화
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}
	return true;
}
void SystemClass::Shutdown()
{
	//graphics객체를 반환
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	//Input 객체를 반환
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}
	//창을 종료
	ShutdownWindows();

	return;
}
void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	//메세지 구조체를 초기화
	ZeroMemory(&msg, sizeof(MSG));

	//유저로부터 종료 메세지를 받을 떄 까지 루프를 돌림
	done = false;
	while (!done)
	{
		//윈도우 메시지를 처리
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//윈도우에서 어플리케이션의 종료를 요청하는 경우 종료
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			//그 외에는 Frame함수를 처리
			result = Frame();
			if (!result)
			{
				MessageBox(m_hwnd, L"Frame Processing Failed", L"Error", MB_OK);
				done = true;
			}
		}
		if (m_Input->IsEscapePressed() == true)
		{
			done = true;
		}
	}
	return;
}
bool SystemClass::Frame()
{
	bool result;
	int mouseX, mouseY;
	
	//입력 프레임 처리
	result = m_Input->Frame();
	if (!result)
	{
		MessageBox(m_hwnd, L"1", L"Error", MB_OK);
		return false;
	}
	
	//마우스 좌표를 가져옴
	m_Input->GetMouseLocation(mouseX, mouseY);
	
	//그래픽객체에서 처리
	result = m_Graphics->Frame(mouseX, mouseY);
	if (!result)
	{
		MessageBox(m_hwnd, L"2", L"Error", MB_OK);
		return false;
	}


	//화면에 그래픽을 렌더링
	result = m_Graphics->Render();
	if (!result)
	{
		return false;
	}
	return true;
}
LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

//렌더링 하게 될 윈도우를 만드는 함수
void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//외부 포인터를 이 객체로 설정
	ApplicationHandle = this;

	//이 어플리 케이션의 인스턴스를 가져옴
	m_hinstance = GetModuleHandle(NULL);

	//이 어플리케이션의 이름을 설정
	m_applicationName = L"Engine";

	//윈도우 클래스를 기본 설정으로 설정
	wc.style		= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc	= WndProc;
	wc.cbClsExtra	= 0;
	wc.cbWndExtra	= 0;
	wc.hInstance	= m_hinstance;
	wc.hIcon		= LoadIcon(NULL,IDI_WINLOGO);
	wc.hIconSm		= wc.hIcon;
	wc.hCursor		= LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName= m_applicationName;
	wc.cbSize		= sizeof(WNDCLASSEX);

	//윈도우 클래스를 등록
	RegisterClassEx(&wc);

	//모니터 화면의 해상도를 가져옴
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//풀스크린 모드 변수의 값에 따라 화면 설정
	if (FULL_SCREEN)
	{
		//만약 풀스크린 모드라면 화면 크기를 데스크톱 크기에 맞추고 색상을 32bit로 설정
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//풀스크린에 맞는 디스플레이 설정
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		//윈도우의 위치를 화면의 왼쪽 위로 설정
		posX = posY = 0;
	}
	else
	{
		//윈도우 모드라면 800x600의 크기를 갖게 함
		screenWidth = 800;
		screenHeight = 600;
		
		//창을 모니터의 중앙에 오도록 함
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//설정한 것을 가지고 창을 만들고 그 핸들을 가져 옴
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, 
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	//윈도우를 화면에 표시하고 포커스를 줌
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//마우스 커서를 표시하지 않음
	ShowCursor(false);

	return;
}
void SystemClass::ShutdownWindows()
{
	//마우스 커서를 표시
	ShowCursor(true);

	//풀스크린 모드를 빠져나올때 디스플레이 설정을 바꿈
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//창을 제거
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//애플리케이션 인스턴스를 제거
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//이 클래스에 대한 외부 포인터 참조를 제거
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch(umsg)
	{
		//윈도우가 제거 되었는지 확인
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		//다른 모든 메세지들은 systemclass의 메세지 처리기에 전달
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
		}
	}
}