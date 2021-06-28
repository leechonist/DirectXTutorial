//3D모델들의 복잡한 기하학들을 캡슐화하는 클래스
//녹색 삼각형을 만드는 클래스

#ifndef _MODEL_CLASS_H
#define _MODEL_CLASS_H

//Include
#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
#include "textureclass.h"
using namespace std;
class ModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();
	
	bool Initialize(ID3D11Device*,char*,const WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, const WCHAR*);
	void ReleaseTexture();
	bool LoadModel(char*);
	void ReleaseModel();

	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	TextureClass* m_texture;
	int m_vertexCount, m_indexCount;
	ModelType* m_model;
};

#endif