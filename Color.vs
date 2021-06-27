//정점 쉐이더

//전역변수
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//타입 정의
//입력값
struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
};

//출력값 PixelShader로 감
struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

//본문
PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;

	//올바르게 행렬 연산을 하기 위하여 position 벡터를 w까지 있는 4성분이 있는 것으로 변환
	input.position.w = 1.0f;

	//정점의 위치를 월드, 뷰, 사영의 순으로 계산
	output.position = mul(input.position,worldMatrix);
	output.position = mul(output.position,viewMatrix);
	output.position = mul(output.position,projectionMatrix);

	//픽셀 쉐이더에서 사용하기 위해 입력 색상을 저장
	output.color = input.color;

	return output;
}