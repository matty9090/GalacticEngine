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
	float3 Sphere : NORMAL1;
	float2 Biome : COLOR0;
	float3 Colour1 : COLOR1;
	float3 Colour2 : COLOR2;
	float3 UV : TEXCOORD0;
    float  NormalIndex : TEXCOORD1;
};

struct VS_INPUT {
	float4 vPosition : POSITION;
	float3 vNormal : NORMAL0;
	float3 vTangent : TANGENT;
	float3 vSphere : NORMAL1;
	float2 vBiome : TEXCOORD0;
	float3 vUV : TEXCOORD1;
    float  vNormalIndex : TEXCOORD2;
};

VS_OUTPUT main(VS_INPUT v_in) {
	VS_OUTPUT Output;
	
	float3 objPos = float3(mWorld[3][0], mWorld[3][1], mWorld[3][2]);
		
	scatter_surf(mul(v_in.vPosition, mWorld).xyz - objPos);

	Output.Position     = mul(v_in.vPosition, mWorldViewProj);
	Output.WorldPos     = mul(v_in.vPosition, mWorld);
	Output.Normal       = mul(v_in.vNormal, mWorld);
	Output.Tangent      = mul(v_in.vTangent, mWorld);
	Output.Sphere       = v_in.vSphere;
	Output.Biome 	    = v_in.vBiome;
	Output.Colour1	    = PrimaryColour;
	Output.Colour2	    = SecondaryColour;
	Output.UV 		    = v_in.vUV;
	Output.NormalIndex  = v_in.vNormalIndex;
	
	//Output.Position.z = LogDepthBuffer(Output.Position.w);

	return Output;
}