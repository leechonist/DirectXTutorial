#ifndef _TEXTURE_CLASS_H
#define _TEXTURE_CLASS_H

#include <d3d11.h>
#include <d3dx11tex.h>

class TextureClass
{
public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(ID3D11Device*, const WCHAR*);
	void Shutdown();
	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11ShaderResourceView* m_texture;
};
#endif