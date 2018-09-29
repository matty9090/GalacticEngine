cbuffer MatrixBuffer : register(b0) {
    matrix mWorldViewProj;
    matrix mWorld;
}

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float3 WorldPos : POSITION;
	float3 Normal : NORMAL0;
	float3 Sphere : NORMAL1;
	float4 Color : COLOR;
	float2 UV : TEXCOORD;
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

	Output.Position = mul(v_in.vPosition, mWorldViewProj);
	Output.WorldPos = mul(v_in.vPosition, mWorld);
	Output.Normal   = mul(v_in.vNormal, mWorld);
	Output.Sphere   = v_in.vSphere;
	Output.Color 	= v_in.vColor;
	Output.UV 		= v_in.vUV;

	return Output;
}