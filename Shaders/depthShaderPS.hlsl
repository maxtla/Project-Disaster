

struct VS_OUT
{
    float4 pos : SV_POSITION;
};

float4 depthshader_ps_main(VS_OUT input) : SV_TARGET
{
    return saturate(float4(0.0f, 0.0f, 0.0f, 1.0f) + (input.pos.z / input.pos.w));
}