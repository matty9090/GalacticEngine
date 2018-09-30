/*
	Adapted from the book GPU Gems
	Author: Sean O'Neil
	Source: https://developer.nvidia.com/gpugems/GPUGems2/gpugems2_chapter16.html
*/

cbuffer MatrixBuffer : register(b0) {
	matrix worldViewProj;
	float3 v3CameraPos;
	float fCameraHeight;
	float3 v3LightDir;
	float fCameraHeight2;
	float3 v3InvWavelength;
	float fOuterRadius;
	float fOuterRadius2;
	float fInnerRadius;
	float fInnerRadius2;
	float fKrESun;
	float fKmESun;
	float fKr4PI;
	float fKm4PI;
	float fScale;
	float fScaleDepth;
	float fScaleOverScaleDepth;
}

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float3 Colour1	: COLOR0;
	float3 Colour2	: COLOR1;
	float3 T0	    : TEXCOORD0;
};

struct VS_INPUT {
	float3 vPosition : POSITION;
};

float scale(float fCos) {
	float x = 1.0 - fCos;
	return 0.25 * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}

VS_OUTPUT main(VS_INPUT v_in) {
	// Get the ray from the camera to the vertex and its length (which
	// is the far point of the ray passing through the atmosphere)
	
	float3 v3Pos = v_in.vPosition * fOuterRadius;
	float3 v3Ray = v3Pos - v3CameraPos;
	float fFar = length(v3Ray);

	v3Ray /= fFar;
	
	// Calculate the closest intersection of the ray with
	// the outer atmosphere (point A in Figure 16-3)

	float fNear = 0.0;
	
	if(fCameraHeight2 > fOuterRadius2) {
		float b = 2.0 * dot(v3CameraPos, v3Ray);
		float c = 4.0 * (fCameraHeight2 - fOuterRadius2);
		float det = max(0.0, b * b - c);
		fNear = 0.5 * (-b - sqrt(det));
	}

	// Calculate the ray's start and end positions in the atmosphere,
	// then calculate its scattering offset

	float3 v3Start = v3CameraPos + v3Ray * fNear;
	fFar -= fNear;
	
	float fStartAngle = dot(v3Ray, v3Start) / fOuterRadius;
	float fStartDepth = exp(-1.0 / fScaleDepth);
	float fStartOffset = fStartDepth * scale(fStartAngle);

	const float fSamples = 3.0f;
	
	// Initialize the scattering loop variables
	float fSampleLength = fFar / fSamples;
	float fScaledLength = fSampleLength * fScale;
	
	float3 v3SampleRay = v3Ray * fSampleLength;
	float3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	// Now loop through the sample points

	float3 v3FrontColor = float3(0.0, 0.0, 0.0);

	for(int i = 0; i < (int)fSamples; i++) {
		float fHeight = length(v3SamplePoint);
		float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
		float fLightAngle = dot(v3LightDir, v3SamplePoint) / fHeight;
		float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
		float fScatter = (fStartOffset + fDepth * (scale(fLightAngle) - scale(fCameraAngle)));

		float3 v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));

		v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
		v3SamplePoint += v3SampleRay;
	}

	// Finally, scale the Mie and Rayleigh colors
	VS_OUTPUT Output;
	
	Output.Position = mul(float4(v_in.vPosition, 1.0f), worldViewProj);
	Output.Colour1	= v3FrontColor * (v3InvWavelength * fKrESun);
	Output.Colour2	= v3FrontColor * fKmESun;
	Output.T0	    = v3CameraPos - v3Pos;

	return Output;
}