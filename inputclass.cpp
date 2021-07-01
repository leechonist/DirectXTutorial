#include "inputclass.h"

InputClass::InputClass()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}
InputClass::InputClass(const InputClass& other)
{
}
InputClass::~InputClass()
{
}

bool InputClass::Initialize(HINSTANCE hinstance,HWND hwnd,int screenWidth,int screenHeight)
{
	HRESULT result;

	//ȭ�� ����� ����
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	//���콺 ��ġ�� �ʱ�ȭ
	m_mouseX = 0;
	m_mouseY = 0;

	//Direct Input �������̽��� �ʱ�ȭ
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"1", L"Error", MB_OK);
		return false;
	}

	//Ű���� �Է� �������̽��� �ʱ�ȭ
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"2", L"Error", MB_OK);
		return false;
	}

	//������ ������ ����
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"3", L"Error", MB_OK);
		return false;
	}

	//Ű���� ���·����� ����
	//DISCL_EXCLUSIVE Ű���带 �ٸ� ���ø����̼ǰ� ����X (���� ����)
	//DISCL_NONEXCLUSIVE Ű���带 �ٸ� ���ø����̼ǰ� ����O (����� ����)
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"4", L"Error", MB_OK);
		return false;
	}

	//���� Ű���带 ȹ��(acquire)
	result = m_keyboard->Acquire();
	if (FAILED(result))
	{
		MessageBox(hwnd, L"5", L"Error", MB_OK);
		return false;
	}

	//���콺 ���̷�Ʈ �Է� �������̽��� �ʱ�ȭ
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"6", L"Error", MB_OK);
		return false;
	}

	//���콺 ������ ������ ����
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"7", L"Error", MB_OK);
		return false;
	}

	//���� ������ ����
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"8", L"Error", MB_OK);
		return false;
	}

	//���콺�� ȹ��
	result = m_mouse->Acquire();
	if (FAILED(result))
	{
		MessageBox(hwnd, L"9", L"Error", MB_OK);
		return false;
	}

	return true;
}
void InputClass::Shutdown()
{
	//���콺�� ����
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	//Ű���带 ����
	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	//���̷�Ʈ ��ǲ ���� �������̽��� ����
	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}

bool InputClass::Frame()
{
	bool result;

	//Ű���� ���� ���¸� ����
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}
	//���콺 ���� ���¸� ����
	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	//���콺�� Ű���� ������ ó��
	ProcessInput();

	return true;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;

	//Ű���� ����̽��� ����
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		//���� Ű���尡 ��Ŀ���� �Ҿ��ų� ȹ�� ���� �ʾҴٸ� ��Ʈ���� �ٽ� ������
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else if (result == DIERR_INVALIDPARAM)
		{
			MessageBox(NULL, L"DIERR_INVALIDPARAM", L"Error", MB_OK);
			return false;
		}
		else if (result == DIERR_NOTINITIALIZED)
		{
			MessageBox(NULL, L"DIERR_NOTINITIALIZED", L"Error", MB_OK);
			return false;
		}
		else if (result == E_PENDING)
		{
			MessageBox(NULL, L"E_PENDING", L"Error", MB_OK);
			return false;
		}
	}
	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;

	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		//���� ���콺�� ��Ŀ���� �Ҿ��ų� ȹ������ �ʾҴٸ� ��Ʈ���� �ٽ� ������
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void InputClass::ProcessInput()
{
	//�ٲ� ������ ������� ���콺 Ŀ���� ��ġ�� ������Ʈ
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	//ȭ�� ���� ������ �ʵ��� Ȯ��
	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth;  }
	if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }

	return;
}

bool InputClass::IsEscapePressed()
{
	//ESC�� ���ȴ��� Ȯ��
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
	return;
}

bool InputClass::IsLeftArrowPressed()
{
	if (m_keyboardState[DIK_LEFTARROW]&0x80)
	{
		return true;
	}
	return false;
}

bool InputClass::IsRightArrowPressed()
{
	if (m_keyboardState[DIK_RIGHTARROW]&0x80)
	{
		return true;
	}
	return false;
}