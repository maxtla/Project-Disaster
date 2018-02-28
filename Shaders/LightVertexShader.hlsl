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
    float3 c_pos : POSITION;
    float2 tex : TEXCOORD0;
};

VS_OUT light_vs_main(VS_IN input)
{
    VS_OUT output;

    input.pos.w = 1.0f;

    output.pos = input.pos;

    output.tex = input.tex;

    //camera pos in world space 
    output.c_pos = float3(view._41, view._42, view._43);

    return output;
}
