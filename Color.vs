//���� ���̴�

//��������
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//Ÿ�� ����
//�Է°�
struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
};

//��°� PixelShader�� ��
struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

//����
PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;

	//�ùٸ��� ��� ������ �ϱ� ���Ͽ� position ���͸� w���� �ִ� 4������ �ִ� ������ ��ȯ
	input.position.w = 1.0f;

	//������ ��ġ�� ����, ��, �翵�� ������ ���
	output.position = mul(input.position,worldMatrix);
	output.position = mul(output.position,viewMatrix);
	output.position = mul(output.position,projectionMatrix);

	//�ȼ� ���̴����� ����ϱ� ���� �Է� ������ ����
	output.color = input.color;

	return output;
}