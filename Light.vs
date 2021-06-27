//전역변수
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

//타입 정의
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
	
	//올바르게 행렬 연산을 하기 위하여 position 벡터를 w까지 있는 4성분이 있는 것으로 변환
	input.position.w = 1.0f;

	//정점의 위치를 월드, 뷰, 사영의 순으로 계산
	output.position = mul(input.position,worldMatrix);
	output.position = mul(output.position,viewMatrix);
	output.position = mul(output.position,projectionMatrix);

	//텍스처 좌표를 픽셀 쉐이더에서 그대로 사용하기 위해 저장
	output.tex = input.tex;

	//월드 행렬만을 사용해 법선 벡터(노멀)을 계산
	output.normal = mul(input.normal,(float3x3)worldMatrix);

	//계산된 법센벡터를 정규화
	output.normal = normalize(output.normal);

	//월드에서 카메라의 위치를 계산
	worldPosition = mul(input.position,worldMatrix);

	//카메라의 위치와 월드에서의 정점 위치를 기반으로 시선 방향을 결정
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

	//시선 방향 벡터를 정규화
	output.viewDirection = normalize(output.viewDirection);

	return output;
}