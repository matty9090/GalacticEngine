#include "Scatter.fx"

cbuffer MatrixBuffer : register(b0) {
    matrix mViewProj;
    matrix mWorld;
    float3 mCamera;
    float  mLerp;
}

struct DS_OUTPUT {
    float4 ProjPos      : SV_Position;
    float3 WorldPos     : POSITION;
    float3 WorldNormal  : NORMAL;
    float3 WorldTangent : TANGENT;
    float2 Biome        : COLOR0;
    float3 Colour1      : COLOR1;
    float3 Colour2      : COLOR2;
    float2 UV           : TEXCOORD0;
    uint   TexIndex     : TEXCOORD1;
};

static const float PI = 3.14159265f;

Texture2D BiomeTex : register(t0);
Texture2D Tex[16] : register(t1);

SamplerState Sampler : register(s0);
	
float4 main(DS_OUTPUT v) : SV_Target {
	float2 tex;
	
	// Spherical UV coordinates
	//tex.x = 0.5f + (atan2(v.Sphere.z, v.Sphere.x) / (2.0f * PI));
	//tex.y = 0.5f - (asin(v.Sphere.y) / PI);
	
	// TODO: Get from buffer
	float3 lightPos = float3(60000.0f, 0.0f, 0.0f);
	float3 lightCol = float3(1.0f, 1.0f, 1.0f);
	float3 ambientCol = float3(0.0f, 0.0f, 0.0f);
	
	float3 lightDir	= normalize(lightPos - v.WorldPos);
	
	// Atmosphere colour contribution
	float3 sColour = v.Colour1 + 0.25f * v.Colour2;
	sColour = 1.0 - exp(sColour * -0.8f);
	
	// Sample biome colour
	float4 biomeCol = BiomeTex.Sample(Sampler, v.Biome);
	
	// Sample texture/normal map colour
	float4 texCol;
	float3 normalMap;
	
	if(v.TexIndex == 0)      { texCol = Tex[0].Sample(Sampler, v.UV); normalMap = Tex[1].Sample(Sampler, v.UV).xyz; }
	else if(v.TexIndex == 2) { texCol = Tex[2].Sample(Sampler, v.UV); normalMap = Tex[3].Sample(Sampler, v.UV).xyz; }
	else if(v.TexIndex == 4) { texCol = Tex[4].Sample(Sampler, v.UV); normalMap = Tex[5].Sample(Sampler, v.UV).xyz; }
	else if(v.TexIndex == 6) { texCol = Tex[6].Sample(Sampler, v.UV); normalMap = Tex[7].Sample(Sampler, v.UV).xyz; }
	else					 { texCol = Tex[0].Sample(Sampler, v.UV); normalMap = Tex[1].Sample(Sampler, v.UV).xyz; }
	
    // Normal mapping
    float3 normal = normalize(v.WorldNormal);
    float3 tangent = normalize(v.WorldTangent - dot(v.WorldTangent, normal) * normal);
    float3 bitangent = cross(normal, tangent);
    float3x3 invTangent = float3x3(tangent, bitangent, normal);

	normalMap = (normalMap * 2.0f) - 1.0f;
    normal = normalize(mul(normalMap, invTangent));
    
    // Lighting
    float3 lightDist = length(lightPos - v.WorldPos);
    float3 diffuseLight = lightCol * max(dot(normal, lightDir), 0.0f);
	
	float3 finalCol = lerp(biomeCol, texCol, mLerp) * diffuseLight;

	return float4(finalCol, 1.0f);
}