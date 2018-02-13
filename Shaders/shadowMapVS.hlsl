cbuffer Matrices : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
    matrix lightView;
    matrix lightProjection;
};

struct VS_IN
{
    float3 pos : POSITION;
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
    float4 lightViewPos : POSITION;
    float3 lightPos : TEXCOORD2;
};

VS_OUT shadowmap_vs_main(VS_IN input)
{
    VS_OUT output;
    float4 _pos = float4(input.pos, 1.0f);

    output.position = mul(_pos, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);

    output.lightViewPos = mul(_pos, world);
    output.lightViewPos = mul(output.lightViewPos, lightView);
    output.lightViewPos = mul(output.lightViewPos, lightProjection);

    output.tex = input.tex;

    output.normal = normalize(mul(input.normal, (float3x3)world));

    float3 _lPos = float3(lightView._41, lightView._42, lightView._43);
    float3 w_pos = mul(input.pos, (float3x3)world);

    output.lightPos = normalize(_lPos - w_pos);

    return output;
}