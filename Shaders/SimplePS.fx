struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float4 Colour   : COLOR;
};
	
float4 main(VS_OUTPUT v) : SV_Target {
	return v.Colour;
}