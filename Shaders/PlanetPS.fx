#include "_Noise.fx"

cbuffer MatrixBuffer : register(b0) {
    matrix mWorldViewProj;
    matrix mWorld;
	
	float frequency;
	float lacunarity;
	float scale;
	float gain;
}

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float3 WorldPos : POSITION;
	float3 Normal : NORMAL;
	float4 Color : COLOR;
};

float GetHeight(float3 p) {
	float v = 0.0f;
	float f = 1.0f;
	float a = 1.0f;
	
	for(int i = 0; i < 8; ++i) {
		v += snoise(p * f) * a;
		f *= 2.0f;
		a *= 0.5f;
	}
	
	return max(v * scale, 0.0f);
}
	
float4 main(VS_OUTPUT v) : SV_Target {
	float3 lightPos = float3(60000.0f, 0.0f, 0.0f);
	float3 lightCol = float3(1.0f, 1.0f, 1.0f);
	float3 ambientCol = float3(0.f, 0.f, 0.f);
	
	float3 worldNormal	= normalize(v.Normal);
	
	float  LightDist	= length(v.WorldPos - lightPos) / 340;
	float3 LightDir		= normalize(lightPos - v.WorldPos);
	float3 DiffuseLight	= lightCol * saturate(dot(worldNormal, LightDir));

	DiffuseLight = ambientCol + DiffuseLight;

	float4 colour;
	colour.rgb = DiffuseLight * v.Color;
	colour.a = 1.0f;

	return colour;
}