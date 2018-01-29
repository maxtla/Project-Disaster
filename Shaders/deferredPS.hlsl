//textures
Texture2D m_texture : register(t0);
//samplers
SamplerState wrapSampler : register(s0);
//structs
struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};
//special struct for output that writes color data to the first render target and normals
//to the second render target
struct PS_OUT
{
    float4 color : SV_Target0;
    float4 normal : SV_Target1;
};

//shader code
PS_OUT deferred_ps_main(VS_OUT input) : SV_TARGET
{
    PS_OUT output;
    //sample color from the texture
    output.color = m_texture.Sample(wrapSampler, input.tex);
    //store the normal
    output.normal = float4(input.normal, 1.0f);

    return output;
}