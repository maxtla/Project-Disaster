cbuffer MatrixBuffer
{
    matrix world;
    matrix view;
    matrix projection;
};

struct VS_IN
{
    float4 pos : POSITION;
    float2 tex : TEXCOORD0;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

VS_OUT light_vs_main(VS_IN input)
{
    VS_OUT output;

    input.pos.w = 1.0f;

    output.pos = mul(input.pos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);

    output.tex = input.tex;

    return output;
}
