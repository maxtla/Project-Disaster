Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);

//point smapler is required for per-pixeldata values from the render textures
SamplerState pointSampler : register(s0);

//constant buffer
cbuffer LightBuffer
{
    float3 lightDir;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 light_ps_main(VS_OUT input) : SV_TARGET
{
    float4 colors;
    float4 normals;
    float3 light_dir;
    float diffuseFactor;
    float4 finalColor;

    colors = colorTexture.Sample(pointSampler, input.tex);
    normals = normalTexture.Sample(pointSampler, input.tex);

    light_dir = -lightDir;
    light_dir = normalize(light_dir);
    normals.xyz = normalize(normals.xyz);

    diffuseFactor = saturate(dot(normals.xyz, light_dir));

    finalColor = saturate(colors * diffuseFactor);

    return finalColor;
}