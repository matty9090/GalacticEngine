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

struct VS_INPUT {
	float4 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float4 vColor : COLOR;
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

VS_OUTPUT main(VS_INPUT v_in) {
	VS_OUTPUT Output;
	
	float3 normal = normalize(v_in.vNormal);	
	float3 pos = v_in.vPosition;
	
	float height = GetHeight(pos);
	pos += normal * height * 0.1f;
	
	//normal = SmoothNormal(pos);

	Output.Position = mul(float4(pos, 1.0f), mWorldViewProj);
	Output.WorldPos = mul(v_in.vPosition, mWorld);
	Output.Normal   = mul(v_in.vNormal, mWorld);
	Output.Color 	= float4(1 / height, 1 / height, 1 / height, 1.0f);

	return Output;
}