Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);

//point smapler is required for per-pixeldata values from the render textures
SamplerState wrapSampler : register(s0);

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
    int hasNormalMap;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 w_pos : POSITION;
    float4 c_pos : TEXCOORD4;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

float4 normalmap_ps_main(VS_OUT input) : SV_TARGET
{
    float4 colors;
    float3 normal;


    colors = diffuseTexture.Sample(wrapSampler, input.tex);
    //Phong Reflection model
    if (true)
     {
         float4 bumMapNormal = normalTexture.Sample(wrapSampler, input.tex);
         float3x3 TBN = float3x3(input.tangent, input.binormal, input.normal);
         bumMapNormal = normalize((2.0f * bumMapNormal) - 1.0f);
         normal = normalize(mul(bumMapNormal, TBN));   
     }
    else
         normal = normalize(input.normal);
        
    float3 lightDir = normalize(lightPos.xyz - input.w_pos.xyz);
    float3 V = normalize(input.c_pos.xyz - input.w_pos.xyz);
    float3 R = normalize(reflect(lightDir, normal));

    float4 Ia = ambient * ambientLight;
    float Id = diffuse * saturate(dot(normal, lightDir));
    float Is = specular * pow(saturate(dot(R, V)), specular_exponent*100);

    float4 I = saturate( (Id + Is) * lightColor);
        
    colors = I * colors;
    

    return colors;
}