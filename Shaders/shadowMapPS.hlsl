Texture2D modelTexture : register(t0);
Texture2D shadowMap : register(t1);

SamplerState wrapSampler : register(s0);
SamplerComparisonState comparisonSampler : register(s1);

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float4 lightViewPos : POSITION;
    float3 lightPos : TEXCOORD2;
};

float4 shadowmap_ps_main(VS_OUT input) : SV_TARGET
{
    float2 projectTexCoord;
    float lightDepthValue;
    float lightning = 1.f;
    float NdotL, margin, epsilon;
    float4 color;

    NdotL = saturate(dot(normalize(input.normal), -1.f*normalize(input.lightPos)));
    margin = acos(saturate(NdotL));
    epsilon = 0.001f / margin;
    epsilon = clamp(epsilon, 0, 0.1);
    //calculate the projected texture coordinats
    projectTexCoord.x = 0.5f + (input.lightViewPos.x / input.lightViewPos.w * 0.5f);
    projectTexCoord.y = 0.5f - (input.lightViewPos.y / input.lightViewPos.w * 0.5f);
    lightDepthValue = input.lightViewPos.z / input.lightViewPos.w; 
   
   if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y) && lightDepthValue > 0)
   {
       lightning = float(shadowMap.SampleCmpLevelZero(comparisonSampler, projectTexCoord, lightDepthValue - epsilon));
       if (lightning == 0)
       {
           return float4(modelTexture.Sample(wrapSampler, input.tex).xyz * 0.3, 1.0f);
       }
       else
       {
           color = modelTexture.Sample(wrapSampler, input.tex);
           return color;
       }
   }

    return float4(0.0f, 0.0f, 0.0f, 1.0f);
}