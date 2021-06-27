#ifndef _FONT_CLASS_H
#define _FONT_CLASS_H

#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
#include "textureclass.h"
using namespace std;

class FontClass
{
private:
	struct FontType
	{
		float left, right;	//ÅØ½ºÃ³ ÁÂÇ¥
		int size;			//ÇÈ¼¿ÀÇ ³Êºñ
	};
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	FontClass();
	FontClass(const FontClass&);
	~FontClass();

	bool Initialize(ID3D11Device*, char*, const WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();
	void BuildVertexArray(void*, char*, float, float);

private:
	bool LoadFontData(char*);
	void ReleaseFontData();
	
	bool LoadTexture(ID3D11Device*, const WCHAR*);
	void ReleaseTexture();

	FontType* m_Font;
	TextureClass* m_Texture;
};

#endif