#include "Common.fx"
#include "Scatter.fx"

cbuffer MatrixBuffer : register(b0) {
    matrix mWorldViewProj;
    matrix mWorld;
    float3 mCam;
    float  mLerp;
    float  mMorph;
}

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float3 WorldPos : POSITION;
	float3 Normal : NORMAL0;
	float3 Tangent : TANGENT;
	float3 Sphere : NORMAL1;
	float2 Biome : COLOR0;
	float3 Colour1 : COLOR1;
	float3 Colour2 : COLOR2;
	float3 UV : TEXCOORD0;
    float  NormalIndex : TEXCOORD1;
};

struct VS_INPUT {
	float3 vPosition1 : POSITION0;
	float3 vPosition2 : POSITION1;
	float3 vNormal1 : NORMAL0;
	float3 vNormal2 : NORMAL1;
	float3 vTangent : TANGENT;
	float3 vSphere : NORMAL2;
	float2 vBiome : TEXCOORD0;
	float3 vUV : TEXCOORD1;
    float  vNormalIndex : TEXCOORD2;
};

VS_OUTPUT main(VS_INPUT v_in) {
	VS_OUTPUT Output;

    float3 finalPos = mul(float4(v_in.vPosition2, 1.0f), mWorld).xyz;
    float  morph = clamp(mMorph / (length(finalPos - mCam)), 0.0f, 1.0f);

    float3 pos = lerp(v_in.vPosition1, v_in.vPosition2, 1.0f);
    float3 normal = lerp(v_in.vNormal1, v_in.vNormal2, 1.0f);
	float3 objPos = float3(mWorld[3][0], mWorld[3][1], mWorld[3][2]);
		
	scatter_surf(mul(float4(pos, 1.0f), mWorld).xyz - objPos);

	Output.Position     = mul(float4(pos, 1.0f), mWorldViewProj);
	Output.WorldPos     = mul(float4(pos, 1.0f), mWorld).xyz;
	Output.Normal       = mul(float4(normal, 1.0f), mWorld).xyz;
	Output.Tangent      = mul(float4(v_in.vTangent, 1.0f), mWorld).xyz;
	Output.Sphere       = v_in.vSphere;
	Output.Biome 	    = v_in.vBiome;
	Output.Colour1	    = PrimaryColour;
	Output.Colour2	    = SecondaryColour;
	Output.UV 		    = v_in.vUV;
	Output.NormalIndex  = v_in.vNormalIndex;
	
	Output.Position.z = LogDepthBuffer(Output.Position.w);

	return Output;
}