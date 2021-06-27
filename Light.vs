//��������
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CamerBuffer
{
	float3 cameraPosition;
	float padding;
};

//Ÿ�� ����
struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};
struct PixelInputType 
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;
	
	//�ùٸ��� ��� ������ �ϱ� ���Ͽ� position ���͸� w���� �ִ� 4������ �ִ� ������ ��ȯ
	input.position.w = 1.0f;

	//������ ��ġ�� ����, ��, �翵�� ������ ���
	output.position = mul(input.position,worldMatrix);
	output.position = mul(output.position,viewMatrix);
	output.position = mul(output.position,projectionMatrix);

	//�ؽ�ó ��ǥ�� �ȼ� ���̴����� �״�� ����ϱ� ���� ����
	output.tex = input.tex;

	//���� ��ĸ��� ����� ���� ����(���)�� ���
	output.normal = mul(input.normal,(float3x3)worldMatrix);

	//���� �������͸� ����ȭ
	output.normal = normalize(output.normal);

	//���忡�� ī�޶��� ��ġ�� ���
	worldPosition = mul(input.position,worldMatrix);

	//ī�޶��� ��ġ�� ���忡���� ���� ��ġ�� ������� �ü� ������ ����
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

	//�ü� ���� ���͸� ����ȭ
	output.viewDirection = normalize(output.viewDirection);

	return output;
}