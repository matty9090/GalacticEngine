struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
};

Texture2D Tex : register(t0);
SamplerState Sampler : register(s0);
	
float4 main(VS_OUTPUT v) : SV_Target {
	return Tex.Sample(Sampler, v.UV);
}