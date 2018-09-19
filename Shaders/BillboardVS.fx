cbuffer MatrixBuffer : register(b0) {
    matrix mWorldViewProj;
}

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
};

struct VS_INPUT {
	float3 Position : POSITION;
	float2 UV : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT v_in) {
	VS_OUTPUT Output;

	Output.Position = mul(float4(v_in.Position, 1.0f), mWorldViewProj);
	Output.UV = v_in.UV;

	return Output;
}