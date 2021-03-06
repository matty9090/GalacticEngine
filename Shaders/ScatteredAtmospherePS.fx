#include "Scatter.fx"

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float3 Colour1	: COLOR0;
	float3 Colour2	: COLOR1;
	float3 T0	    : TEXCOORD0;
};

float4 main(VS_OUTPUT v) : SV_Target {
	float3 colour = getScatterColour(v.Colour1, v.Colour2, v.T0);
	colour = 1.0 - exp(colour * -0.8f);
	
	return float4(colour, colour.b);
}