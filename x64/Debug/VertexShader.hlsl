cbuffer SceneConstantBuffer : register(b0)
{
	float4 offset;
};

struct VertexInput
{
	float4 position		: POSITION;
	float4 color		: COLOR;
};

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
};

VertexToPixel main(VertexInput input)
{
	VertexToPixel output;
	output.position = input.position + offset;
	output.color = input.color;

	return output;
}