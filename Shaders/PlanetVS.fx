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
};

struct VS_INPUT {
	float4 vPosition : POSITION;
	float3 vNormal : NORMAL0;
    float3 vTangent : TANGENT;
	float3 vSphere : NORMAL1;
	float2 vBiome : TEXCOORD0;
	float2 vUV : TEXCOORD1;
	uint   vTexIndex : TEXCOORD2;
};

HS_INPUT main(VS_INPUT v_in) {
    HS_INPUT Output;
	
	float3 objPos = float3(mWorld[3][0], mWorld[3][1], mWorld[3][2]);
	
	scatter_surf(mul(v_in.vPosition, mWorld).xyz - objPos);

	Output.WorldPos     = mul(v_in.vPosition, mWorld);
	Output.WorldNormal  = mul(v_in.vNormal, mWorld);
	Output.WorldTangent = mul(v_in.vTangent, mWorld);
	Output.Biome 	    = v_in.vBiome;
	Output.Colour1	    = PrimaryColour;
	Output.Colour2	    = SecondaryColour;
	Output.UV 		    = v_in.vUV;
	Output.TexIndex	    = v_in.vTexIndex;

	return Output;
}