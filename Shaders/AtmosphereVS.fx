#include "Common.fx"

cbuffer MatrixBuffer : register(b0) {
	matrix   viewProj;
	matrix   world;
	float3   lightDir;
	float	 planetRadius;
	float3   camPos;
	float	 atmosphereRadius;
	float3   atmColour;
	float	 camHeight;
}

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float3 Colour	: COLOR0;
	float Alpha		: COLOR1;
};

struct VS_INPUT {
	float4 vPosition : POSITION;
};

float getFirstRaySphereIntersection(float3 rayOrigin, float3 rayDir, float radius) {
	float b = 2.0 * dot(rayOrigin, rayDir);
	float c = dot(rayOrigin, rayOrigin) - radius * radius;
	float discriminant = sqrt(max(0.0, b * b - 4.0 * c));
	return -0.5 * b - 0.5 * discriminant;
}

VS_OUTPUT main(VS_INPUT v_in) {
	VS_OUTPUT Output;

	float3 rayEnd = v_in.vPosition * atmosphereRadius;
	float3 rayDir = normalize(rayEnd - camPos);
	float3 rayStart = camPos;

	if (camHeight > atmosphereRadius)
		rayStart += rayDir * getFirstRaySphereIntersection(camPos, rayDir, atmosphereRadius);

	float rayLength = distance(rayEnd, rayStart);
	float atmosphereHeight = atmosphereRadius - planetRadius;
	float alpha = rayLength / atmosphereRadius;
    
	alpha = 0.0f;

	float stepSize = rayLength / float(5);

	for (int i = 0; i < 5; i++) {
		float3 rayStepPos = rayStart + float(i) * stepSize * rayDir;
		float rayStepPosHeight = length(rayStepPos);
		float3 rayStepDir = rayStepPos / rayStepPosHeight;
		float density = 1.0 - (rayStepPosHeight - planetRadius) / atmosphereHeight;
		density -= 0.2;
		float light = max(0.0, 0.2 + dot(rayStepDir, lightDir));
		alpha += light * density;
	}

	alpha /= float(5);
	alpha *= 1.4;

	Output.Position = mul(float4(rayEnd, 1.0), world);
	Output.Position = mul(Output.Position, viewProj);
	Output.Colour	= atmColour;
	Output.Alpha	= alpha;
	
	Output.Position.z = LogDepthBuffer(Output.Position.w);

	return Output;
}