//3D모델들의 복잡한 기하학들을 캡슐화하는 클래스
//녹색 삼각형을 만드는 클래스

#ifndef _BITMAP_CLASS_H
#define _BITMAP_CLASS_H

//Include
#include <d3d11.h>
#include <d3dx10math.h>

#include "textureclass.h"
using namespace std;
class BitmapClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	BitmapClass();
	BitmapClass(const BitmapClass&);
	~BitmapClass();
	
	bool Initialize(ID3D11Device*, int, int, const WCHAR*, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, const WCHAR*);
	void ReleaseTexture();

	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	TextureClass* m_texture;
	int m_vertexCount, m_indexCount;
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;

};

#endif