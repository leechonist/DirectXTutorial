#ifndef _GRAPHICS_CLASS_H
#define _GRAPHICS_CLASS_H

#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "lightmapshaderclass.h"

//전역변수
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass& other);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
	bool Render();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	LightMapShaderClass* m_LightMapShader;
};

#endif