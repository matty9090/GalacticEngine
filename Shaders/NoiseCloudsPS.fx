#include "Scatter.fx"

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float4 Colour	: COLOR0;
	float3 Colour1 : COLOR1;
	float3 Colour2 : COLOR2;
};
	
float4 main(VS_OUTPUT v) : SV_Target {
	float3 sColour = v.Colour1 + 0.25f * v.Colour2;
	sColour = 1.0 - exp(sColour * -0.8f);

	float4 col = v.Colour + float4(sColour, 0.0f);
	col *= sColour.b;
	
	return col;
}