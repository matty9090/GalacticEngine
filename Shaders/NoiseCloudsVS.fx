#include "Common.fx"
#include "Noise.fx"
#include "Scatter.fx"

cbuffer MatrixBuffer : register(b0) {
	matrix   viewProj;
	matrix   world;
	float3   lightDir;
	float    time;
}

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float4 Colour	: COLOR0;
	float3 Colour1 : COLOR1;
	float3 Colour2 : COLOR2;
};

struct VS_INPUT {
	float3 vPosition : POSITION;
};

VS_OUTPUT main(VS_INPUT v_in) {
	VS_OUTPUT Output;
	
	float3 pos = v_in.vPosition;
	pos.z += time * 20.0f;
	
	float3 npos = pos * 2;
	
	float col = snoise(npos);
	col += snoise(npos * 2) * 0.5f;
	col += snoise(npos * 4) * 0.25f;
	col += snoise(npos * 8) * 0.125f;
	
	float3 objPos = float3(world[3][0], world[3][1], world[3][2]);
	scatter_surf(mul(v_in.vPosition, world).xyz - objPos);

	Output.Position = mul(float4(v_in.vPosition, 1.0f), world);
	Output.Position = mul(Output.Position, viewProj);
	Output.Colour	= float4(col, col, col, col);
	Output.Colour1	= PrimaryColour;
	Output.Colour2	= SecondaryColour;
	
	//Output.Position.z = LogDepthBuffer(Output.Position.w);

	return Output;
}