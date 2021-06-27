#include "cursorclass.h"
CursorClass::CursorClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}
CursorClass::CursorClass(const CursorClass& other)
{
}
CursorClass::~CursorClass()
{
}

bool CursorClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight)
{
	bool result;
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	m_previousPosX = -1;
	m_previousPosY = -1;

	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void CursorClass::Shutdown()
{
	ShutdownBuffers();
	return;
}

void CursorClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result;
	result = UpdateBuffers(deviceContext, positionX, positionY);

}