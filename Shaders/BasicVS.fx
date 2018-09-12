cbuffer MatrixBuffer : register(b0) {
    matrix mWorldViewProj;
}

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
};

struct VS_INPUT {
	float4 vPosition : POSITION;
	float4 vColor : COLOR;
};

VS_OUTPUT main(VS_INPUT v_in) {
	VS_OUTPUT Output;

	Output.Position = mul(v_in.vPosition, mWorldViewProj);
	Output.Color = v_in.vColor;

	return Output;
}