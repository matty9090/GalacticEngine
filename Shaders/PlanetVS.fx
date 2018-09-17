cbuffer MatrixBuffer : register(b0) {
    matrix mWorldViewProj;
    matrix mWorld;
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

VS_OUTPUT main(VS_INPUT v_in) {
	VS_OUTPUT Output;

	Output.Position = mul(v_in.vPosition, mWorldViewProj);
	Output.WorldPos = mul(v_in.vPosition, mWorld);
	Output.Normal   = mul(v_in.vNormal, mWorld);
	Output.Color 	= v_in.vColor;

	return Output;
}