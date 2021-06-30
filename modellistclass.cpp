#include "modellistclass.h"
ModelListClass::ModelListClass()
{
	m_ModelInfoList = 0;
}
ModelListClass::ModelListClass(const ModelListClass& other)
{
}
ModelListClass::~ModelListClass()
{
}

bool ModelListClass::Initialize(int numModels)
{
	int i;
	float red, green, blue;
	
	//���� ���� ����
	m_modelCount = numModels;

	//�� ������ ����Ʈ �迭�� ����
	m_ModelInfoList = new ModelInfoType[m_modelCount];
	if (!m_ModelInfoList)
	{
		return false;
	}

	//���� �������� seed�� ���� �ð����� ����
	srand((unsigned int)time(NULL));

	//��� �𵨵��� ����� ��ġ�� ���Ƿ� ���Ͽ� �� �迭�� ����
	for (i = 0; i < m_modelCount; i++)
	{
		//���� ������ ���Ƿ� ����
		red = (float)rand() / RAND_MAX;
		green = (float)rand() / RAND_MAX;
		blue = (float)rand() / RAND_MAX;

		m_ModelInfoList[i].color = D3DXVECTOR4(red,green,blue,1.0f);
		
		//���� ��ġ�� ���Ƿ� ����
		m_ModelInfoList[i].positionX = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionY = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionZ = ((((float)rand() - (float)rand()) / RAND_MAX) * 10.0f)+5.00f;
	}
	return true;
}

void ModelListClass::Shutdown()
{
	//�� ���� ����Ʈ�� ����
	if (m_ModelInfoList)
	{
		delete[] m_ModelInfoList;
		m_ModelInfoList = 0;
	}

	return;
}

int ModelListClass::GetModelCount()
{
	return m_modelCount;
}

void ModelListClass::GetData(int index, float& positionX, float& positionY, float& positionZ, D3DXVECTOR4& color)
{
	positionX = m_ModelInfoList[index].positionX;
	positionY = m_ModelInfoList[index].positionY;
	positionZ = m_ModelInfoList[index].positionZ;

	color = m_ModelInfoList[index].color;
	
	return;
}
//Done!