cbuffer EXAMPLE_BUFFER : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 proj;
}

struct GS_OUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};
struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

[maxvertexcount(3)]
void bill_gs_main(triangle VS_OUT input[3], inout TriangleStream<GS_OUT> OutputStream)
{
	GS_OUT output[3];
	float3 m_e1 = float3(input[1].Pos.x - input[0].Pos.x,
						input[1].Pos.y - input[0].Pos.y
						input[1].Pos.z - input[0].Pos.z);
	float3 m_e2 = float3(input[2].Pos.x - input[0].Pos.x,
						input[2].Pos.y - input[0].Pos.y,
						input[2].Pos.z - input[0].Pos.z);
	float3 m_normal = normalize(cross(m_e1, m_e2));

	for (int i = 0; i < 3; i++)
	{
		output[i].Pos = mul(input[i].Pos, world);
		output[i].Pos = mul(output[i].Pos, view);
		output[i].Pos = mul(output[i].Pos, proj);
		output[i].Normal = mul(float4(m_normal, 0), world).xyz;
		output[i].Tex = input[i].Tex;
		outputStream.append(output[i]);
	}
}