Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D positionTexture : register(t2);

//point smapler is required for per-pixeldata values from the render textures
SamplerState pointSampler : register(s0);

//constant buffer
cbuffer LightBuffer : register(b0)
{
    float4 lightPos;
    float4 lightColor;
    float4 ambientLight;
};

cbuffer MaterialBuffer : register(b1)
{
    float ambient;
    float diffuse;
    float specular;
    float specular_exponent;
    bool hasNormalMap;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float3 c_pos : POSITION;
    float2 tex : TEXCOORD0;
};

float4 light_ps_main(VS_OUT input) : SV_TARGET
{
    float4 colors;
    float4 normals;
    float4 positions;

    colors = colorTexture.Sample(pointSampler, input.tex);
    normals = normalTexture.Sample(pointSampler, input.tex);
    positions = positionTexture.Sample(pointSampler, input.tex);

    //Phong Rion model
    if (length(normals.xyz) > 0.0f)
    {
        float3 normal = normalize(normals.xyz);
        float3 lightDir = normalize(lightPos.xyz - positions.xyz);
        float3 V = normalize(input.c_pos - positions.xyz);
        float3 R = normalize(reflect(lightDir, normal));

        float4 Ia = ambient * ambientLight;
        float Id = diffuse * saturate(dot(normal, lightDir));
        float Is = specular * pow(saturate(dot(R, V)), specular_exponent);

        float4 I = saturate(Ia + (Id + Is) * lightColor);
        colors = I * colors;
    }

    return colors;
}