#include "fontclass.h"

FontClass::FontClass()
{
	m_Font = 0;
	m_Texture = 0;
}

FontClass::FontClass(const FontClass& other)
{
}

FontClass::~FontClass()
{
}

bool FontClass::Initialize(ID3D11Device* device, char* fontFilename, const WCHAR* textureFilename)
{
	bool result;

	//폰트 데이터를 가지고 있는 텍스트 파일 열기
	result = LoadFontData(fontFilename);
	if (!result)
	{
		return false;
	}

	//폰트 문자를 가지고 있는 텍스처를 열기
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void FontClass::Shutdown()
{
	//폰트 텍스처를 해제
	ReleaseTexture();

	//폰트 데이터를 해제
	ReleaseFontData();
}

bool FontClass::LoadFontData(char* filename)
{
	ifstream fin;
	int i;
	char temp;

	//폰트 공간 버퍼를 생성
	m_Font = new FontType[95];
	if (!m_Font)
	{
		return false;
	}
	
	//파일을 열기
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	//텍스트 정보를 읽음
	for (i = 0; i < 95; i++)
	{
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}
	//파일을 닫기
	fin.close();

	return true;
}
void FontClass::ReleaseFontData()
{
	//폰트 데이터 배열을 해제
	if (m_Font)
	{
		delete[] m_Font;
		m_Font = 0;
	}

	return;
}

bool FontClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	bool result;
	
	//텍스처 오브젝트 생성
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	//텍스처 오브젝트 초기화
	result = m_Texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void FontClass::ReleaseTexture()
{
	//텍스처 오브젝트 해제
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}
ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return m_Texture->GetTexture();
}

void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter;

	//vertuces를 정점 타입 구조체에 저장
	vertexPtr = (VertexType*)vertices;

	//문자열의 길이 저장
	numLetters = (int)strlen(sentence);

	//정점 배열의 인덱스 초기화
	index = 0;

	for (i = 0; i < numLetters; i++)
	{
		letter = ((int)sentence[i] - 32);

		//만약 문자가 스페이스 공백이라면 3픽셀 옮기기
		if (letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			//첫번째 삼각형
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX+m_Font[letter].size, drawY-16, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 1.0f);
			index++;
			
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY-16, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 1.0f);
			index++;

			//두번째 삼각형
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX + m_Font[letter].size, drawY, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX + m_Font[letter].size, drawY - 16, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 1.0f);
			index++;

			//길이를 문자 길이 + 1픽셀 추가
			drawX = drawX + m_Font[letter].size + 1.0f;
		}
	}
	return;
}
