cbuffer SceneConstantBuffer : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
};

struct VertexInput
{
	float3 position		: POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
};

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
};

VertexToPixel main(VertexInput input)
{
	VertexToPixel output;

	matrix worldViewProj = mul(mul(world, view), projection);


	output.position = mul(float4(input.position, 1.0f), worldViewProj);
	output.normal = input.normal;

	return output;
}