cbuffer MatrixBuffer : register(b0) {
    matrix mWorldViewProj;
}

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float4 Colour : COLOR;
};

struct VS_INPUT {
	float3 vPosition : POSITION;
	float4 vColour : COLOR;
};

VS_OUTPUT main(VS_INPUT v_in) {
	VS_OUTPUT Output;

	Output.Position = mul(float4(v_in.vPosition, 1.0f), mWorldViewProj);
	Output.Colour = v_in.vColour;

	return Output;
}