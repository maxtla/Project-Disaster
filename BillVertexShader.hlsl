
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

	output.pos = float4(input.pos, 1.0);
	output.tex = input.tex;
	

	return output;
}