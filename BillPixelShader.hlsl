Texture2D txture : register(t0);
SamplerState sampAni : register(s0);

cbuffer FS_CONST_BUFFER : register(b1)
{
	float4 lightdir;
	float4 lightcol;
	float3 cameraPos;
}
struct PS_OUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};
float4 bill_ps_main(PS_OUT input) : SV_TARGET
{
	float3 normal = normalize(input.Normal);
	float3 lightNormal = normalize(lightdir.xyz);
	float4 pColor = txture.Sample(sampAni, input.Tex);
	
	return pColor*lightcol;
}