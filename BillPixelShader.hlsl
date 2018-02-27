Texture2D txture : register(t0);
SamplerState sampAni : register(s0);


struct PS_OUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
};
float4 bill_ps_main(PS_OUT input) : SV_TARGET
{
	float4 textureColor;
	
	textureColor = txture.Sample(sampAni, input.tex);
	
	return textureColor;
}