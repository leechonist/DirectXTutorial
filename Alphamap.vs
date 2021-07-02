
//��������
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//Ÿ�� ����
struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PixelInputType AlphaMapVertexShader(VertexInputType Input)
{
	PixelInputType output;

	//������ ���� 4��° ��ҿ� 1 �߰�
	Input.position.w = 1.0f;

	//��ġ�� ����ϱ� ���� ����,��, ���� ����� ���Ͽ� ���
	output.position = mul(Input.position,worldMatrix);
	output.position = mul(output.position,viewMatrix);
	output.position = mul(output.position,projectionMatrix);

	//�ؽ�ó ��ǥ�� �����Ͽ� �״�� ����
	output.tex = Input.tex;

	return output;
}