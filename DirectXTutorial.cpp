#include "systemclass.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;

	//system 객체를 생성
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}
	
	//system 객체를 초기화하고 new을 호출
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	//system객체를 종료하고 메모리를 반환
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}