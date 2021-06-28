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

	//화면 사이즈를 저장
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	//마우스 위치를 초기화
	m_mouseX = 0;
	m_mouseY = 0;

	//Direct Input 인터페이스를 초기화
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"1", L"Error", MB_OK);
		return false;
	}

	//키보드 입력 인터페이스를 초기화
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"2", L"Error", MB_OK);
		return false;
	}

	//데이터 포맷을 설정
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"3", L"Error", MB_OK);
		return false;
	}

	//키보드 협력레벨을 설정
	//DISCL_EXCLUSIVE 키보드를 다른 어플리케이션과 공유X (배제 상태)
	//DISCL_NONEXCLUSIVE 키보드를 다른 어플리케이션과 공유O (비배제 상태)
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"4", L"Error", MB_OK);
		return false;
	}

	//이제 키보드를 획득(acquire)
	result = m_keyboard->Acquire();
	if (FAILED(result))
	{
		MessageBox(hwnd, L"5", L"Error", MB_OK);
		return false;
	}

	//마우스 다이렉트 입력 인터페이스를 초기화
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"6", L"Error", MB_OK);
		return false;
	}

	//마우스 데이터 포맷을 설정
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"7", L"Error", MB_OK);
		return false;
	}

	//협력 레벨을 설정
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"8", L"Error", MB_OK);
		return false;
	}

	//마우스를 획득
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
	//마우스를 해제
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	//키보드를 해제
	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	//다이렉트 인풋 메인 인터페이스를 해제
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

	//키보드 현재 상태를 읽음
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}
	//마우스 현재 상태를 읽음
	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	//마우스와 키보드 변경을 처리
	ProcessInput();

	return true;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;

	//키보드 디바이스를 읽음
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		//만약 키보드가 포커스를 잃었거나 획득 하지 않았다면 컨트롤을 다시 가져옴
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
		//만약 마우스가 포커스를 잃었거나 획득하지 않았다면 컨트롤을 다시 가져옴
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
	//바뀐 내용을 기반으로 마우스 커서의 위치를 업데이트
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	//화면 밖을 나가지 않도록 확인
	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth;  }
	if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }

	return;
}

bool InputClass::IsEscapePressed()
{
	//ESC가 눌렸는지 확인
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