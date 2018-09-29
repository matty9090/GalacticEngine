cbuffer MatrixBuffer : register(b0) {
	float3 v3LightDir;
	float g;
	float g2;
}

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float3 Colour1	: COLOR0;
	float3 Colour2	: COLOR1;
	float3 T0	    : TEXCOORD0;
};

// Calculates the Mie phase function
float getMiePhase(float fCos, float fCos2, float g, float g2) {
	return 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos2) / pow(1.0 + g2 - 2.0 * g * fCos, 1.5);
}

// Calculates the Rayleigh phase function
float getRayleighPhase(float fCos2) {
	return 0.75 + 0.75 * fCos2;
}

float4 main(VS_OUTPUT v) : SV_Target {
	float fCos = dot(v3LightDir, v.T0) / length(v.T0);
	float fCos2 = fCos * fCos;

	float3 colour = getRayleighPhase(fCos2) * v.Colour1 + getMiePhase(fCos, fCos2, g, g2) * v.Colour2;
	
	return float4(colour, colour.b);
}