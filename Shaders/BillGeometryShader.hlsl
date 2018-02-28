cbuffer Matrices : register(b0)
{
	float4x4 worldPos;
	float4x4 view;
	float4x4 proj;
	float3 cameraPos;
}

struct GS_OUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
};
struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

[maxvertexcount(4)]
void bill_gs_main(triangle VS_OUT input[3], inout TriangleStream<GS_OUT> OutputStream)
{
	//Old shit will soon be deleted
	/*GS_OUT output[3];
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
	}*/
	float halfWidth = snowWidth / 2.0f;

	float3 planeNormal = input[0].world - cameraPos;
	planeNormal.y = 0.0f;
	planeNormal = normalize(planeNormal);
	float3 upVector = float3(0.0f, 1.0f, 0.0f);
	float3 rightVector = normalize(cross(planeNormal, upVector));

	rightVector = rightVector * halfWidth;

	upVector = float3(0, halfSnowHeight, 0);

	float3 vert[4];

	//We get the points by using the billboards right vector and the billboard height
	//might need new variable worldPos to replace world here
	vert[0] = input[0].world - rightVector;
	vert[1] = input[0].world + rightVector;
	vert[2] = input[0].world - rightVector + upVector;
	vert[3] = input[0].world + rightVector + upVector;

	//Billboard texture coords
	//not sure if billboard will have a texture. might just let it be white
	float2 texCoord[4];
	texCoord[0] = float2(0, 1);
	texCoord[1] = float2(1, 1);
	texCoord[2] = float2(0, 0);
	texCoord[3] = float2(1, 0);

	VS_OUTPUT outputVert;
	for (int i = 0; i < 4; i++)
	{
		outputVert.pos = mul(float4(vert[i], 1.0f)world);
		outputVert.world = float4(vert[i], 0.0f); //maybe worldPos
		outputVert.tex = texCoord[i];
		outputstream.append(outputVert);
	}

}