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
    float4 w_pos : POSITION;
    float2 tex : TEXCOORD0;
    float4 normal : NORMAL;
};

//shader code
VS_OUT deferred_vs_main(VS_IN input)
{
    VS_OUT output;
    //adjust pos for proper matrix multiplications
    output.pos.w = 1.0f;
    //do matrix calculations
    float4 position = float4(input.pos, 1.0f);
    output.pos = mul(position, world);
    //position in world space
    output.w_pos = output.pos;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    //put the normal in world space
    float4 norm = float4(input.normal, 1.0f);
    output.normal = mul(norm, world);
    output.normal = normalize(output.normal);
    //pass through the tex coord
    output.tex = input.tex;

    return output;
}