#include "Common.fx"
#include "Scatter.fx"

cbuffer MatrixBuffer : register(b0) {
    matrix mViewProj;
    matrix mWorld;
    float3 mCamera;
    float  mLerp;
}

struct HS_INPUT {
	float3 WorldPos : POSITION;
	float3 WorldNormal : NORMAL0;
	float3 WorldTangent : TANGENT;
	float2 Biome : COLOR0;
	float3 Colour1 : COLOR1;
	float3 Colour2 : COLOR2;
	float2 UV : TEXCOORD0;
	uint   TexIndex : TEXCOORD1;
    float  Tess : TESSFACTOR;
};

struct VS_INPUT {
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
	float2 vBiome : TEXCOORD0;
	float2 vUV : TEXCOORD1;
    float  vTess : TESSFACTOR;
	uint   vTexIndex : TEXCOORD2;
};

HS_INPUT main(VS_INPUT v_in) {
    HS_INPUT Output;
	
	float3 objPos = float3(mWorld[3][0], mWorld[3][1], mWorld[3][2]);
	
	scatter_surf(mul(float4(v_in.vPosition, 1.0f), mWorld).xyz - objPos);

	Output.WorldPos     = mul(float4(v_in.vPosition, 1.0f), mWorld).xyz;
	Output.WorldNormal  = mul(float4(v_in.vNormal, 1.0f), mWorld).xyz;
	Output.WorldTangent = mul(float4(v_in.vTangent, 1.0f), mWorld).xyz;
	Output.Biome 	    = v_in.vBiome;
	Output.Colour1	    = PrimaryColour;
	Output.Colour2	    = SecondaryColour;
	Output.UV 		    = v_in.vUV;
	Output.TexIndex	    = v_in.vTexIndex;
	Output.Tess	        = v_in.vTess;

	return Output;
}