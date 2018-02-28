Texture2D colorTexture : register(t0);
Texture2D normalMap : register(t1);

SamplerState wrapSampler : register(s0);

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

float4 heightmap_ps_main(VS_OUT input) : SV_TARGET
{
    float3 m_normal;
    float3 dirLight = float3(0.2f, 1.0f, 0.5f);
    float4 lightColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    dirLight = normalize(dirLight);

    float4 bumpNormal = normalMap.Sample(wrapSampler, input.tex);
    float3x3 TBN = float3x3(input.tangent, input.binormal, input.normal);
    bumpNormal = normalize((2.0f * bumpNormal) - 1.0f);
    m_normal = normalize(mul(bumpNormal.xyz, TBN));

    float Id = saturate(dot(m_normal, dirLight));

    float4 I = saturate(Id * lightColor);

    return saturate(colorTexture.Sample(wrapSampler, input.tex) * I);
    //return float4(1.0f, 0.0f, 0.0f, 1.0f);
}