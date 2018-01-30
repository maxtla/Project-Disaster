Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D positionTexture : register(t2);

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
    float3 c_pos : POSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
};

float4 light_ps_main(VS_OUT input) : SV_TARGET
{
    float4 colors;
    float4 normals;
    float3 light_dir;
    float diffuseFactor;
    float4 finalColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    float3 c_position;

    colors = colorTexture.Sample(pointSampler, input.tex);
    normals = normalTexture.Sample(pointSampler, input.tex);
    input.pos = positionTexture.Sample(pointSampler, input.tex);
    if (length(normals) > 0.0f)
    {
        light_dir = -lightDir;
        light_dir = normalize(light_dir);
        c_position = input.c_pos;
        diffuseFactor = saturate(dot(normals.xyz, light_dir));
        float specular = 0.0f;
        if (diffuseFactor > 0.0f)
        {
            normals.xyz = normalize(normals.xyz);
            float3 viewDir = normalize(-c_position);
            float3 halfDir = normalize(lightDir + viewDir);
            float specAngle = saturate(dot(halfDir, normals));
            specular = pow(specAngle, 0.1f);
        }
        float3 colorLinear = diffuseFactor * colors.xyz + specular * float3(1.0f, 1.0f, 1.0f);
        finalColor = float4(pow(colorLinear, float3(1.0f/2.2f, 1.0f/2.2f, 1.0f/2.2f)), 1.0f);

    }

    return finalColor;
}