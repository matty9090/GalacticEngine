#include "Scatter.fx"

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float3 WorldPos : POSITION;
	float3 Normal : NORMAL0;
	float3 Tangent : TANGENT;
	float3 Sphere : NORMAL1;
	float2 Biome : COLOR0;
	float3 Colour1 : COLOR1;
	float3 Colour2 : COLOR2;
	float2 UV : TEXCOORD0;
	uint   TexIndex : TEXCOORD1;
};

static const float PI = 3.14159265f;

Texture2D BiomeTex : register(t0);
Texture2D Tex[8] : register(t1);

SamplerState Sampler : register(s0);
	
float4 main(VS_OUTPUT v) : SV_Target {
	float2 tex;
	
	// Spherical UV coordinates
	tex.x = 0.5f + (atan2(v.Sphere.z, v.Sphere.x) / (2.0f * PI));
	tex.y = 0.5f - (asin(v.Sphere.y) / PI);
	
	// TODO: Get from buffer
	float3 lightPos = float3(60000.0f, 0.0f, 0.0f);
	float3 lightCol = float3(1.0f, 1.0f, 1.0f);
	float3 ambientCol = float3(0.f, 0.f, 0.f);
	
	float3 lightDir	= normalize(lightPos - v.WorldPos);
	
	// Normal mapping
	float3 normal  = normalize(v.Normal);
	float3 tangent  = normalize(v.Tangent);
	float3 binormal = cross(normal, tangent);

	// Atmosphere colour contribution
	float3 sColour = v.Colour1 + 0.25f * v.Colour2;
	sColour = 1.0 - exp(sColour * -0.8f);
	
	// Sample biome colour
	float4 col = BiomeTex.Sample(Sampler, v.Biome);
	//col *= sColour.b;
	
	// Sample texture/normal map colour
	float4 diffuse;
	float4 normalMap;
	
	if(v.TexIndex == 0)      { diffuse = Tex[0].Sample(Sampler, v.UV); normalMap = Tex[1].Sample(Sampler, v.UV); }
	else if(v.TexIndex == 2) { diffuse = Tex[2].Sample(Sampler, v.UV); normalMap = Tex[3].Sample(Sampler, v.UV); }
	else if(v.TexIndex == 4) { diffuse = Tex[4].Sample(Sampler, v.UV); normalMap = Tex[5].Sample(Sampler, v.UV); }
	else if(v.TexIndex == 6) { diffuse = Tex[6].Sample(Sampler, v.UV); normalMap = Tex[7].Sample(Sampler, v.UV); }
	else					 { diffuse = Tex[0].Sample(Sampler, v.UV); normalMap = Tex[1].Sample(Sampler, v.UV); }
	
	normalMap = (normalMap * 2.0f) - 1.0f;
	float3 bumpNormal = (normalMap.x * tangent) + (normalMap.y * binormal) + (normalMap.z * normal);
	bumpNormal = normalize(bumpNormal);
	float lightIntensity = saturate(dot(bumpNormal, lightDir));
	
	float3 finalCol = col * diffuse;
	//finalCol = saturate(finalCol * lightIntensity);

	float4 colour;
	colour.rgb = finalCol;
	//colour.rgb = col;
	colour.a = 1.0f;

	return colour;
}