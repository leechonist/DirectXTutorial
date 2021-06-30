#include "frustumclass.h"

FrustumClass::FrustumClass()
{
}
FrustumClass::FrustumClass(const FrustumClass& other)
{
}
FrustumClass::~FrustumClass()
{
}
void FrustumClass::ConstructFrustum(float screenDepth, D3DXMATRIX projectionMatrix, D3DXMATRIX viewMatrix)
{
	float zMinimum, r;
	D3DXMATRIX matrix;

	//퓨러스텀에서 최소 Z의 거리를 계산
	zMinimum = -projectionMatrix._43 / projectionMatrix._33;
	r = screenDepth / (screenDepth - zMinimum);

	projectionMatrix._33 = r;
	projectionMatrix._43 = -r * zMinimum;

	//뷰 행렬과 업데이트된 투영 행렬로 퓨러스텀 행렬을 생성
	D3DXMatrixMultiply(&matrix, &viewMatrix, &projectionMatrix);

	//퓨러스텀의 가까운 평면을 계산
	m_planes[0].a = matrix._14 + matrix._13;
	m_planes[0].b = matrix._24 + matrix._23;
	m_planes[0].c = matrix._34 + matrix._33;
	m_planes[0].d = matrix._44 + matrix._43;
	D3DXPlaneNormalize(&m_planes[0], &m_planes[0]);

	//퓨러스텀의 먼 평면을 계산
	m_planes[1].a = matrix._14 - matrix._13;
	m_planes[1].b = matrix._24 - matrix._23;
	m_planes[1].c = matrix._34 - matrix._33;
	m_planes[1].d = matrix._44 - matrix._43;
	D3DXPlaneNormalize(&m_planes[1], &m_planes[1]);

	//퓨러스텀의 좌측 평면을 계산
	m_planes[2].a = matrix._14 + matrix._11;
	m_planes[2].b = matrix._24 + matrix._21;
	m_planes[2].c = matrix._34 + matrix._31;
	m_planes[2].d = matrix._44 + matrix._41;
	D3DXPlaneNormalize(&m_planes[2], &m_planes[2]);

	//퓨러스텀의 우측 평면을 계산
	m_planes[3].a = matrix._14 - matrix._11;
	m_planes[3].b = matrix._24 - matrix._21;
	m_planes[3].c = matrix._34 - matrix._31;
	m_planes[3].d = matrix._44 - matrix._41;
	D3DXPlaneNormalize(&m_planes[3], &m_planes[3]);


	//퓨러스텀의 윗 평면을 계산
	m_planes[4].a = matrix._14 - matrix._12;
	m_planes[4].b = matrix._24 - matrix._22;
	m_planes[4].c = matrix._34 - matrix._32;
	m_planes[4].d = matrix._44 - matrix._42;
	D3DXPlaneNormalize(&m_planes[4], &m_planes[4]);

	//퓨러스텀의 아래 평면을 계산
	m_planes[5].a = matrix._14 + matrix._12;
	m_planes[5].b = matrix._24 + matrix._22;
	m_planes[5].c = matrix._34 + matrix._32;
	m_planes[5].d = matrix._44 + matrix._42;
	D3DXPlaneNormalize(&m_planes[5], &m_planes[5]);

	return;
}

bool FrustumClass::CheckPoint(float x, float y, float z)
{
	int i;
	D3DXVECTOR3 point = D3DXVECTOR3(x, y, z);
	//점이 뷰 프러스텀의 6면 내에 있는지 확인
	for (i = 0; i < 6; i++)
	{
		if (D3DXPlaneDotCoord(&m_planes[i], &point) < 0.0f)
		{
			return false;
		}
	}
	return true;
}

bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;
	D3DXVECTOR3 point;
	//정육면체의 8개 중 하나의 점이 뷰프러스텀 내에 있는지 확인
	for (i = 0; i < 6; i++)
	{
		point = D3DXVECTOR3((xCenter - radius), (yCenter - radius), (zCenter - radius));
		if (D3DXPlaneDotCoord(&m_planes[i], &point) >= 0.0f)
		{
			continue;
		}
		point = D3DXVECTOR3((xCenter + radius), (yCenter - radius), (zCenter - radius));
		if (D3DXPlaneDotCoord(&m_planes[i], &point) >= 0.0f)
		{
			continue;
		}
		point = D3DXVECTOR3((xCenter - radius), (yCenter + radius), (zCenter - radius));
		if (D3DXPlaneDotCoord(&m_planes[i], &point) >= 0.0f)
		{
			continue;
		}
		point = D3DXVECTOR3((xCenter + radius), (yCenter + radius), (zCenter - radius));
		if (D3DXPlaneDotCoord(&m_planes[i], &point) >= 0.0f)
		{
			continue;
		}
		point = D3DXVECTOR3((xCenter - radius), (yCenter - radius), (zCenter + radius));
		if (D3DXPlaneDotCoord(&m_planes[i], &point) >= 0.0f)
		{
			continue;
		}
		point = D3DXVECTOR3((xCenter + radius), (yCenter - radius), (zCenter + radius));
		if (D3DXPlaneDotCoord(&m_planes[i], &point) >= 0.0f)
		{
			continue;
		}
		point = D3DXVECTOR3((xCenter - radius), (yCenter + radius), (zCenter + radius));
		if (D3DXPlaneDotCoord(&m_planes[i], &point) >= 0.0f)
		{
			continue;
		}
		point = D3DXVECTOR3((xCenter + radius), (yCenter + radius), (zCenter + radius));
		if (D3DXPlaneDotCoord(&m_planes[i], &point) >= 0.0f)
		{
			continue;
		}
		return false;
	}
	return true;
}

bool FrustumClass::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;
	D3DXVECTOR3 point = D3DXVECTOR3(xCenter, yCenter, zCenter);
	//퓨러스텀 평면에 구의 중심이 반지름 만큼의 반경에 존재하는지 확인
	for (i = 0; i < 6; i++)
	{
		if (D3DXPlaneDotCoord(&m_planes[i], &point) < -radius)
		{
			return false;
		}
	}
	return true;
}

bool FrustumClass::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	int i;
	D3DXVECTOR3 point;

	//삼각형이 퓨러스텀 안에 있는지 확인
	for (i = 0; i < 6; i++)
	{
		point = D3DXVECTOR3((xCenter - xSize), (yCenter - ySize), (zCenter - zSize));
		if (D3DXPlaneDotCoord(&m_planes[i], &point) >= 0.0f)
		{
			continue;
		}
		point = D3DXVECTOR3((xCenter + xSize), (yCenter - ySize), (zCenter - zSize));
		if (D3DXPlaneDotCoord(&m_planes[i], &point) >= 0.0f)
		{
			continue;
		}
		point = D3DXVECTOR3((xCenter - xSize), (yCenter + ySize), (zCenter - zSize));
		if (D3DXPlaneDotCoord(&m_planes[i], &point) >= 0.0f)
		{
			continue;
		}
		point = D3DXVECTOR3((xCenter + xSize), (yCenter + ySize), (zCenter - zSize));
		if (D3DXPlaneDotCoord(&m_planes[i], &point) >= 0.0f)
		{
			continue;
		}
		point = D3DXVECTOR3((xCenter - xSize), (yCenter - ySize), (zCenter + zSize));
		if (D3DXPlaneDotCoord(&m_planes[i], &point) >= 0.0f)
		{
			continue;
		}
		point = D3DXVECTOR3((xCenter + xSize), (yCenter - ySize), (zCenter + zSize));
		if (D3DXPlaneDotCoord(&m_planes[i], &point) >= 0.0f)
		{
			continue;
		}
		point = D3DXVECTOR3((xCenter - xSize), (yCenter + ySize), (zCenter + zSize));
		if (D3DXPlaneDotCoord(&m_planes[i], &point) >= 0.0f)
		{
			continue;
		}
		point = D3DXVECTOR3((xCenter + xSize), (yCenter + ySize), (zCenter + zSize));
		if (D3DXPlaneDotCoord(&m_planes[i], &point) >= 0.0f)
		{
			continue;
		}
		return false;
	}
	return true;
}