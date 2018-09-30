struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float3 WorldPos : POSITION;
	float3 Normal : NORMAL0;
	float3 Sphere : NORMAL1;
	float4 Color : COLOR;
	float2 UV : TEXCOORD;
};

static const float PI = 3.14159265f;

Texture2D Tex : register(t0);
Texture2D Surf : register(t1);
SamplerState Sampler : register(s0);
	
float4 main(VS_OUTPUT v) : SV_Target {
	float2 tex;
	
	tex.x = 0.5f + (atan2(v.Sphere.z, v.Sphere.x) / (2.0f * PI));
	tex.y = 0.5f - (asin(v.Sphere.y) / PI);
	
	float3 lightPos = float3(60000.0f, 0.0f, 0.0f);
	float3 lightCol = float3(1.0f, 1.0f, 1.0f);
	float3 ambientCol = float3(0.f, 0.f, 0.f);
	
	float3 worldNormal	= normalize(v.Normal);
	
	float  LightDist	= length(v.WorldPos - lightPos) / 340;
	float3 LightDir		= normalize(lightPos - v.WorldPos);
	float3 DiffuseLight	= lightCol * saturate(dot(worldNormal, LightDir));

	DiffuseLight = ambientCol + DiffuseLight;

	float4 colour;
	colour.rgb = Tex.Sample(Sampler, tex) /** Surf.Sample(Sampler, v.UV)*/ *  DiffuseLight;
	colour.a = 1.0f;

	return colour;
}