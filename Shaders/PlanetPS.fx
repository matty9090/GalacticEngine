struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float3 WorldPos : POSITION;
	float3 Normal : NORMAL;
	float4 Color : COLOR;
};
	
float4 main(VS_OUTPUT v) : SV_Target {
	float3 lightPos = float3(100.0f, 50.0f, -100.0f);
	float3 lightCol = float3(1.0f, 1.0f, 1.0f);
	float3 ambientCol = float3(0.f, 0.f, 0.f);
	
	float3 worldNormal	= normalize(v.Normal);
	
	float  LightDist	= length(v.WorldPos - lightPos) / 340;
	float3 LightDir		= normalize(lightPos - v.WorldPos);
	float3 DiffuseLight	= lightCol * saturate(dot(worldNormal, LightDir));

	DiffuseLight = ambientCol + DiffuseLight;

	float4 colour;
	colour.rgb = DiffuseLight * v.Color;
	colour.a = 1.0f;

	return colour;
}