
#include "LightsHelper.hlsli"

cbuffer LightData : register(b1)
{
	float4 AmbientLight;
	float3 EyePos;
	float pad;
	Light lights[MaxLights];
};

cbuffer MaterialData : register(b2)
{
	float4 DiffuseAlbedo;
	float3 FresnelR0;
	float Roughness;
};

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float3 worldPos		: WORLDPOS;
	float2 uv			: TEXCOORD;
};

Texture2D diffuseAlbedo		: register(t0);
SamplerState basicSampler	: register(s0);

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	// Grab the texture color
	float4 textureColor = diffuseAlbedo.Sample(basicSampler, input.uv);

	// Renormalize interpolated normals
	input.normal = normalize(input.normal);

	// Vector from point being lit to eye
	float3 toEye = normalize(EyePos - input.worldPos);

	// Indirect lighting
	float4 ambient = AmbientLight * DiffuseAlbedo;

	const float shininess = 1.0f - Roughness;
	Material mat = { DiffuseAlbedo, FresnelR0 , shininess };
	float3 shadowFactor = 1.0f;

	float4 resultLight = ComputeLighting(lights, mat, input.worldPos, input.normal, toEye, shadowFactor);

	float4 litColor = ambient + resultLight;

	litColor.a = DiffuseAlbedo.a;

	return litColor*textureColor;
}