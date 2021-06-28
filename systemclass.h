#pragma once
#ifndef _SYSTEM_CLASS_H
#define _SYSTEM_CLASS_H

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "inputclass.h"
#include "graphicsclass.h"
#include "timerclass.h"
#include "position.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	GraphicsClass* m_Graphics;
	TimerClass* m_Timer;
	PositionClass* m_Position;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//전역변수
static SystemClass* ApplicationHandle = 0;
#endif
