cbuffer Vector : register(b0)
{
    float4 camPos;
}

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 w_pos : POSITION;
    float2 tex : TEXCOORD0;
    float4 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

[maxvertexcount(3)]
void deferred_gs_main(triangle VS_OUT input[3], inout TriangleStream<VS_OUT> in_out_stream)
{
    //test the vertices normals against the camToTriangle vector to see if this triangle is
    //in front of us or behind, if it is in front append, if behind discard triangle
    /*float d;
    bool cull = false;
    float3 camToVertex = normalize(camPos.xyz - input[0].w_pos.xyz);

    d = dot(camToVertex, input[0].normal);
    if (d < 0.0f)
        cull = true;
    

    if (!cull)
    {
        in_out_stream.Append(input[0]);
        in_out_stream.Append(input[1]);
        in_out_stream.Append(input[2]);
    }*/

    //calculate the normal in 2D space
    bool cull = false;
    float3 edge1 = (input[1].pos - input[0].pos);
    float3 edge2 = (input[2].pos - input[0].pos);

    float3 m_normal = normalize(cross(edge1, edge2));

    if (m_normal.z > 0.f)
        cull = true;

    if (!cull)
    {
        in_out_stream.Append(input[0]);
        in_out_stream.Append(input[1]);
        in_out_stream.Append(input[2]);
    }
    //both seem to work, altough not great, these approaches are implement from the info from the lecture slides about backface culling

    //try out reflection parity based technique found on wikipedia
    /*bool cull = false;
    matrix <float,3, 3> m = { input[1].pos.x - input[0].pos.x, input[2].pos.x - input[0].pos.x, input[0].pos.x, //row 1
                              input[1].pos.y - input[0].pos.y, input[2].pos.y - input[0].pos.y, input[0].pos.y, //row 2
                              0.f, 0.f, 1.0f }; //row3

    float det = determinant(m);
    if (det > 0.f)
        cull = true;

    if (!cull)
    {
        in_out_stream.Append(input[0]);
        in_out_stream.Append(input[1]);
        in_out_stream.Append(input[2]);
    } */ //this also worked equally good as the projection approach 
}