struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
};
	
float4 main(VS_OUTPUT v) : SV_Target {
	float4 colour;
	colour.rgb = v.Color;
	colour.a = 1.0f;

	return colour;
}