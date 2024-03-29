#include "textureclass.h"

TextureClass::TextureClass()
{
	m_texture = 0;
}
TextureClass::TextureClass(const TextureClass&)
{
}
TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, const WCHAR* filename)
{
	HRESULT result;

	//텍스처파일을 불러옴
	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_texture, NULL);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TextureClass::Shutdown()
{
	//텍스처를 해제
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	return;
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_texture;
}
