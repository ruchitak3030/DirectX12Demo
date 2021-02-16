#define MaxLights 3

// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 1
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 1
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 1
#endif

struct Light
{
	float3 Strength;
	float FallOffStart;		// point/spot light only
	float3 Direction;		// directional/spot light only
	float FallOffEnd;		// point/spot light only
	float3 Position;		// Point Light only
	float SpotPower;		// Spot light only
};

struct Material
{
	float4 DiffuseAlbedo;
	float3 FresnelR0;
	float Shininess;
};

float CalcAttenuation(float d, float fallOffStart, float fallOffEnd)
{
	return saturate((fallOffEnd - d) / (fallOffEnd - fallOffStart));
}

float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
	float cosIncidentAngle = saturate(dot(normal, lightVec));

	float f0 = 1.0f - cosIncidentAngle;
	float3 reflectPercent = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);

	return reflectPercent;
}

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat)
{
	const float m = mat.Shininess * 256.0f;
	float3 halfVec = normalize(toEye + lightVec);

	float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
	float3 fresnelFactor = SchlickFresnel(mat.FresnelR0, halfVec, lightVec);

	float3 specAlbedo = fresnelFactor * roughnessFactor;

	specAlbedo = specAlbedo / (specAlbedo + 1.0f);

	return (mat.DiffuseAlbedo.rgb + specAlbedo) * lightStrength;
}

float3 ComputeDirectionalLight(Light L, Material mat, float3 normal, float3 toEye)
{
	// The light vector aims opposite to the direction of light rays travel
	float3 lightVec = -L.Direction;

	// Scale light down by Lambert's cosine law
	float NdotL = max(dot(lightVec, normal), 0.0f);
	float3 lightStrength = L.Strength * NdotL;

	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}


float3 ComputePointLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
	// The vector from the surface to the light
	float3 lightVec = L.Position - pos;

	// the distance from the surface to the light
	float d = length(lightVec);

	// Range test
	if (d > L.FallOffEnd)
		return 0.0f;

	// Normalize the light vec
	lightVec /= d;

	// Scale light by lambert's cosine law.
	float NdotL = max(dot(lightVec, normal), 0.0f);
	float3 lightStrength = L.Strength * NdotL;

	// Attenuate light by distance
	float att = CalcAttenuation(d, L.FallOffStart, L.FallOffEnd);
	lightStrength *= att;

	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

float3 ComputeSpotLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
	// The vector from the surface to the light
	float3 lightVec = L.Position - pos;

	// the distance from the surface to the light
	float d = length(lightVec);

	// Range test
	if (d > L.FallOffEnd)
		return 0.0f;

	// Normalize the light vec
	lightVec /= d;

	// Scale light by lambert's cosine law.
	float NdotL = max(dot(lightVec, normal), 0.0f);
	float3 lightStrength = L.Strength * NdotL;

	// Attenuate light by distance
	float att = CalcAttenuation(d, L.FallOffStart, L.FallOffEnd);
	lightStrength *= att;


	// Scale by spotLight
	float spotFactor = pow(max(dot(-lightVec, L.Direction), 0.0f), L.SpotPower);
	lightStrength *= spotFactor;

	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);

}

float4 ComputeLighting(Light lights[MaxLights], Material mat,
	float3 pos, float3 normal, float3 toEye,
	float3 shadowFactor)
{
	float3 litColor = 0.0f;
	int i = 0;
#if (NUM_DIR_LIGHTS > 0)
	for (i = 0; i < NUM_DIR_LIGHTS; ++i)
	{
		litColor += shadowFactor[i] * ComputeDirectionalLight(lights[i], mat, normal, toEye);
	}
#endif

#if (NUM_POINT_LIGHTS > 0)
	for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
	{
		litColor += ComputePointLight(lights[i], mat, pos, normal, toEye);
	}	
#endif

#if (NUM_SPOT_LIGHTS > 0)
	for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
	{
		litColor += ComputeSpotLight(lights[i], mat, pos, normal, toEye);
	}
#endif

	return float4(litColor, 1.0f);
}