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

	//��Ʈ �����͸� ������ �ִ� �ؽ�Ʈ ���� ����
	result = LoadFontData(fontFilename);
	if (!result)
	{
		return false;
	}

	//��Ʈ ���ڸ� ������ �ִ� �ؽ�ó�� ����
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void FontClass::Shutdown()
{
	//��Ʈ �ؽ�ó�� ����
	ReleaseTexture();

	//��Ʈ �����͸� ����
	ReleaseFontData();
}

bool FontClass::LoadFontData(char* filename)
{
	ifstream fin;
	int i;
	char temp;

	//��Ʈ ���� ���۸� ����
	m_Font = new FontType[95];
	if (!m_Font)
	{
		return false;
	}
	
	//������ ����
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	//�ؽ�Ʈ ������ ����
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
	//������ �ݱ�
	fin.close();

	return true;
}
void FontClass::ReleaseFontData()
{
	//��Ʈ ������ �迭�� ����
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
	
	//�ؽ�ó ������Ʈ ����
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	//�ؽ�ó ������Ʈ �ʱ�ȭ
	result = m_Texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void FontClass::ReleaseTexture()
{
	//�ؽ�ó ������Ʈ ����
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

	//vertuces�� ���� Ÿ�� ����ü�� ����
	vertexPtr = (VertexType*)vertices;

	//���ڿ��� ���� ����
	numLetters = (int)strlen(sentence);

	//���� �迭�� �ε��� �ʱ�ȭ
	index = 0;

	for (i = 0; i < numLetters; i++)
	{
		letter = ((int)sentence[i] - 32);

		//���� ���ڰ� �����̽� �����̶�� 3�ȼ� �ű��
		if (letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			//ù��° �ﰢ��
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX+m_Font[letter].size, drawY-16, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 1.0f);
			index++;
			
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY-16, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 1.0f);
			index++;

			//�ι�° �ﰢ��
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX + m_Font[letter].size, drawY, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX + m_Font[letter].size, drawY - 16, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 1.0f);
			index++;

			//���̸� ���� ���� + 1�ȼ� �߰�
			drawX = drawX + m_Font[letter].size + 1.0f;
		}
	}
	return;
}
