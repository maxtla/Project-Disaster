cbuffer Matrices : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
}

struct VS_IN
{
	float3 Pos : POSITION;
	float4 Color : COLOR;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

VS_OUT mousepicking_vs_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = float4(input.Pos, 1);
	output.Pos = mul(output.Pos, world);
	output.Pos = mul(output.Pos, view);
	output.Pos = mul(output.Pos, projection);
	output.Color = input.Color;

	return output;
}