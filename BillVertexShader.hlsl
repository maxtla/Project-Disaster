
cbuffer Matrices : register(b0) 
{
	matrix world;
	matrix view;
	matrix proj;
};

struct VS_IN
{
	float3 pos : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float4 normal : NORMAL;

};
VS_OUT bill_vs_main(VS_IN input)
{
	VS_OUT output;

	//output.pos.w = 1.0f;

	output.pos = mul(input.pos, world);
	output.pos = mul(input.pos, view);
	output.pos = mul(input.pos, proj);

	output.tex = input.tex;

	return output;
}