cbuffer LightData : register(b1)
{
	float4 directionalLightColor;
	float3 directionalLightDirection;
	float pad;
	float4 pointLightColor;
	float3 pointLightPosition;
	float pad1;
	float3 cameraPosition;
	float pad2;
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

	// Standard N dot L lighting (direction TO the light)
	float lightAmountDL = saturate(dot(input.normal, -directionalLightDirection));

	// N dot L for point light
	float3 dirToPointLight = normalize(pointLightPosition - input.worldPos);
	float lightAmountPL = saturate(dot(input.normal, dirToPointLight));

	// Specular highlight for point light
	float3 toCamera = normalize(cameraPosition - input.worldPos);
	float3 refl = reflect(-dirToPointLight, input.normal);
	float specular = pow(saturate(dot(refl, toCamera)), 8);

	return textureColor;
	/*(directionalLightColor * lightAmountDL * textureColor) +
	(pointLightColor * lightAmountPL * textureColor) +
	specular;*/
}