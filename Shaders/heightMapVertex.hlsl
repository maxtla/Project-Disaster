cbuffer Matrices : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

struct VS_IN
{
    float3 position : POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

VS_OUT heightmap_vs_main(VS_IN input)
{
    VS_OUT output;

    float4 pos = float4(input.position, 1.0f);
    output.position = mul(pos, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);

    output.tangent = normalize(input.tangent);
    output.binormal = normalize(input.binormal);
    output.tangent = mul(output.tangent, (float3x3)world);
    output.binormal = mul(output.binormal, (float3x3)world);

    output.normal = normalize(mul(input.normal, (float3x3)world));
    output.tex = input.tex;

    return output;
}