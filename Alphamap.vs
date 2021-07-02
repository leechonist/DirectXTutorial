
//전역변수
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//타입 정의
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

	//연산을 위해 4번째 요소에 1 추가
	Input.position.w = 1.0f;

	//위치를 계산하기 위해 월드,뷰, 투영 행렬을 곱하여 계산
	output.position = mul(Input.position,worldMatrix);
	output.position = mul(output.position,viewMatrix);
	output.position = mul(output.position,projectionMatrix);

	//텍스처 좌표를 복사하여 그대로 전달
	output.tex = Input.tex;

	return output;
}