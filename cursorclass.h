#pragma once
#ifndef _CURSOR_CLASS_H
#define _CURSOR_CLASS_H

#include <d3d11.h>
#include <d3dx10math.h>
class CursorClass
{
	CursorClass();
	CursorClass(const CursorClass&);
	~CursorClass();

	bool Initialize(ID3D11Device*, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);

	int GetIndexCount();
private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);
	
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	int m_screenWidth, m_screenHeight;
	int m_previousPosX, m_previousPosY;

};
#endif
