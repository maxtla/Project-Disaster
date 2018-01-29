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
    float4 pos : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
}

//shader code
VS_OUT deferred_vs_main(VS_IN input)
{
    VS_OUT output;
    //adjust pos for proper matrix multiplications
    input.pos.w = 1.0f;
    //do matrix calculations
    output.pos = mul(input.pos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    //put the normal in world space
    output.normal = mul(input.normal, (float3x3)world);
    output.normal = normalize(output.normal);
    //pass through the tex coord
    output.tex = input.tex;

    return output;
}