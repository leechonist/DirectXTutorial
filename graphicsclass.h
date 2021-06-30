#ifndef _GRAPHICS_CLASS_H
#define _GRAPHICS_CLASS_H

#include "d3dclass.h"
#include "cameraclass.h"
#include "textclass.h"
#include "modelclass.h"
#include "lightshaderclass.h"
#include "lightclass.h"
#include "modellistclass.h"
#include "frustumclass.h"

//전역변수
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
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
	bool Frame(float);
	bool Render();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	TextClass* m_Text;
	ModelClass* m_Model;
	LightShaderClass* m_LightShader;
	LightClass* m_Light;
	ModelListClass* m_ModelList;
	FrustumClass* m_Frustum;

};
#endif