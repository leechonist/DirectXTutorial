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
	
	//모델의 수를 저장
	m_modelCount = numModels;

	//모델 정보의 리스트 배열을 생성
	m_ModelInfoList = new ModelInfoType[m_modelCount];
	if (!m_ModelInfoList)
	{
		return false;
	}

	//난수 생성기의 seed를 현재 시간으로 설정
	srand((unsigned int)time(NULL));

	//모든 모델들의 색상과 위치를 임의로 정하여 모델 배열에 저장
	for (i = 0; i < m_modelCount; i++)
	{
		//모델의 색상을 임의로 결정
		red = (float)rand() / RAND_MAX;
		green = (float)rand() / RAND_MAX;
		blue = (float)rand() / RAND_MAX;

		m_ModelInfoList[i].color = D3DXVECTOR4(red,green,blue,1.0f);
		
		//모델의 위치를 임의로 결정
		m_ModelInfoList[i].positionX = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionY = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionZ = ((((float)rand() - (float)rand()) / RAND_MAX) * 10.0f)+5.00f;
	}
	return true;
}

void ModelListClass::Shutdown()
{
	//모델 정보 리스트를 해제
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