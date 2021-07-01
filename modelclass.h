//3D모델들의 복잡한 기하학들을 캡슐화하는 클래스
//녹색 삼각형을 만드는 클래스

#ifndef _MODEL_CLASS_H
#define _MODEL_CLASS_H

//Include
#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
#include "texturearrayclass.h"
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
	
	bool Initialize(ID3D11Device*,char*,WCHAR*,WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView** GetTextureArray();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTextures(ID3D11Device*,WCHAR*,WCHAR*);
	void ReleaseTextures();
	bool LoadModel(char*);
	void ReleaseModel();

	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	TextureArrayClass* m_textureArray;
	int m_vertexCount, m_indexCount;
	ModelType* m_model;
};

#endif