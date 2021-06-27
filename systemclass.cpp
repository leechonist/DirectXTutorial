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

	//�Լ��� ���̿� �ʺ� �����ϱ� ���� ������ 0���� �ʱ�ȭ
	screenWidth = screenHeight = 0;

	InitializeWindows(screenWidth, screenHeight);

	//input ��ü�� ���� Ű���� �Է��� ó���ϱ� ���� ���
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}
	
	//Input ��ü�� �ʱ�ȭ
	result = m_Input->Initialize(m_hinstance,m_hwnd,screenWidth,screenHeight);
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	//graphics ��ü�� ���� �׷��� ��Ҹ� �׸��� ���� ���
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	//graphics ��ü�� �ʱ�ȭ
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}
	return true;
}
void SystemClass::Shutdown()
{
	//graphics��ü�� ��ȯ
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	//Input ��ü�� ��ȯ
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}
	//â�� ����
	ShutdownWindows();

	return;
}
void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	//�޼��� ����ü�� �ʱ�ȭ
	ZeroMemory(&msg, sizeof(MSG));

	//�����κ��� ���� �޼����� ���� �� ���� ������ ����
	done = false;
	while (!done)
	{
		//������ �޽����� ó��
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//�����쿡�� ���ø����̼��� ���Ḧ ��û�ϴ� ��� ����
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			//�� �ܿ��� Frame�Լ��� ó��
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
	
	//�Է� ������ ó��
	result = m_Input->Frame();
	if (!result)
	{
		MessageBox(m_hwnd, L"1", L"Error", MB_OK);
		return false;
	}
	
	//���콺 ��ǥ�� ������
	m_Input->GetMouseLocation(mouseX, mouseY);
	
	//�׷��Ȱ�ü���� ó��
	result = m_Graphics->Frame(mouseX, mouseY);
	if (!result)
	{
		MessageBox(m_hwnd, L"2", L"Error", MB_OK);
		return false;
	}


	//ȭ�鿡 �׷����� ������
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

//������ �ϰ� �� �����츦 ����� �Լ�
void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//�ܺ� �����͸� �� ��ü�� ����
	ApplicationHandle = this;

	//�� ���ø� ���̼��� �ν��Ͻ��� ������
	m_hinstance = GetModuleHandle(NULL);

	//�� ���ø����̼��� �̸��� ����
	m_applicationName = L"Engine";

	//������ Ŭ������ �⺻ �������� ����
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

	//������ Ŭ������ ���
	RegisterClassEx(&wc);

	//����� ȭ���� �ػ󵵸� ������
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//Ǯ��ũ�� ��� ������ ���� ���� ȭ�� ����
	if (FULL_SCREEN)
	{
		//���� Ǯ��ũ�� ����� ȭ�� ũ�⸦ ����ũ�� ũ�⿡ ���߰� ������ 32bit�� ����
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Ǯ��ũ���� �´� ���÷��� ����
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		//�������� ��ġ�� ȭ���� ���� ���� ����
		posX = posY = 0;
	}
	else
	{
		//������ ����� 800x600�� ũ�⸦ ���� ��
		screenWidth = 800;
		screenHeight = 600;
		
		//â�� ������� �߾ӿ� ������ ��
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//������ ���� ������ â�� ����� �� �ڵ��� ���� ��
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, 
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	//�����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ���� ��
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//���콺 Ŀ���� ǥ������ ����
	ShowCursor(false);

	return;
}
void SystemClass::ShutdownWindows()
{
	//���콺 Ŀ���� ǥ��
	ShowCursor(true);

	//Ǯ��ũ�� ��带 �������ö� ���÷��� ������ �ٲ�
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//â�� ����
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//���ø����̼� �ν��Ͻ��� ����
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//�� Ŭ������ ���� �ܺ� ������ ������ ����
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch(umsg)
	{
		//�����찡 ���� �Ǿ����� Ȯ��
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
		//�ٸ� ��� �޼������� systemclass�� �޼��� ó���⿡ ����
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
		}
	}
}