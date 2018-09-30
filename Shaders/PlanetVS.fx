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
	float3 Sphere : NORMAL1;
	float4 Color : COLOR0;
	float3 Colour1 : COLOR1;
	float3 Colour2 : COLOR2;
	float2 UV : TEXCOORD0;
};

struct VS_INPUT {
	float4 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float3 vSphere : TANGENT;
	float4 vColor : COLOR;
	float2 vUV : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT v_in) {
	VS_OUTPUT Output;
	
	float3 objPos = float3(mWorld[3][0], mWorld[3][1], mWorld[3][2]);
		
	scatter_surf(mul(v_in.vPosition, mWorld).xyz - objPos);

	Output.Position = mul(v_in.vPosition, mWorldViewProj);
	Output.WorldPos = mul(v_in.vPosition, mWorld);
	Output.Normal   = mul(v_in.vNormal, mWorld);
	Output.Sphere   = v_in.vSphere;
	Output.Color 	= v_in.vColor;
	Output.Colour1	= PrimaryColour;
	Output.Colour2	= SecondaryColour;
	Output.UV 		= v_in.vUV;
	
	//Output.Position.z = LogDepthBuffer(Output.Position.w);

	return Output;
}