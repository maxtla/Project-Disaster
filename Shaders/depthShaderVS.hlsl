//describe constant buffer layout
cbuffer Matrices : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

//define structs for input and output
struct VS_IN
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};


struct VS_OUT
{
    float4 pos : SV_POSITION;
};

VS_OUT depthshader_vs_main(VS_IN input)
{
    VS_OUT output;

    float4 _pos = float4(input.pos, 1.0f); //for proper matrix calculations

    output.pos = mul(_pos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);

    return output;
}