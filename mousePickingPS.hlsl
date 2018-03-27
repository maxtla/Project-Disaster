struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

float4 mousepicking_ps_main(VS_OUT input) : SV_TARGET
{
	return input.Color;
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
}