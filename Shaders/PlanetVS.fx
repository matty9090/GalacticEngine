#include "Common.fx"
#include "Scatter.fx"

cbuffer MatrixBuffer : register(b0) {
    matrix mWorldViewProj;
    matrix mWorld;
}

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float3 WorldPos : POSITION;
	float3 Normal : NORMAL0;
	float3 Tangent : TANGENT;
	float2 Biome : COLOR0;
	float3 Colour1 : COLOR1;
	float3 Colour2 : COLOR2;
	float3 UV : TEXCOORD0;
    float  NormalIndex : TEXCOORD1;
};

struct VS_INPUT {
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float3 vTangent : TANGENT;
	float2 vBiome : TEXCOORD0;
	float3 vUV : TEXCOORD1;
    float  vNormalIndex : TEXCOORD2;
};

VS_OUTPUT main(VS_INPUT v_in) {
	VS_OUTPUT Output;
	
	float3 objPos = float3(mWorld[3][0], mWorld[3][1], mWorld[3][2]);
		
	scatter_surf(mul(float4(v_in.vPosition, 1.0f), mWorld).xyz - objPos);

	Output.Position     = mul(float4(v_in.vPosition, 1.0f), mWorldViewProj);
	Output.WorldPos     = mul(float4(v_in.vPosition, 1.0f), mWorld).xyz;
	Output.Normal       = mul(float4(v_in.vNormal, 1.0f), mWorld).xyz;
	Output.Tangent      = mul(float4(v_in.vTangent, 1.0f), mWorld).xyz;
	Output.Biome 	    = v_in.vBiome;
	Output.Colour1	    = PrimaryColour;
	Output.Colour2	    = SecondaryColour;
	Output.UV 		    = v_in.vUV;
	Output.NormalIndex  = v_in.vNormalIndex;
	
	Output.Position.z = LogDepthBuffer(Output.Position.w);

	return Output;
}