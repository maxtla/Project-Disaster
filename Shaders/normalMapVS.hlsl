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
    nointerpolation float4 c_pos : TEXCOORD4;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

//shader code
VS_OUT normalmap_vs_main(VS_IN input)
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

    //pass through the tex coord
    output.tex = input.tex;

    output.c_pos = float4(view._41, view._42, view._43, 0.0f);

    output.tangent = normalize(input.tangent);
    output.binormal = normalize(input.binormal);
    output.tangent = mul(output.tangent, (float3x3)world);
    output.binormal = mul(output.binormal, (float3x3)world);

    output.normal = normalize(mul(input.normal, (float3x3)world));


    return output;
}