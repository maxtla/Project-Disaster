#include "QuadTree.h"



QuadTree::QuadTree()
{
	m_parent = nullptr;
}


QuadTree::~QuadTree()
{
}

bool QuadTree::initialize(vector<unsigned int> indices, int mapSize, float delta, ID3D11Device * pDev)
{
	m_indices = indices;
	m_mapSize = mapSize;
	deltaSpace = delta;

	//create the parent node
	m_parent = new NodeTypeQuadTree;
	if (m_parent == nullptr)
		return false;

	//recursively build the quad tree
	createTreeNode(m_parent, 0, 0, mapSize, pDev);

	//once the tree is build and the vertices are stored in the vertex buffers, release the vector to free up memory space
	indices.clear();
	m_indices.clear();

	return true;
}

void QuadTree::Release()
{
	if (m_parent)
	{
		ReleaseNode(m_parent);
		delete m_parent;
		m_parent = nullptr;
	}
}

void QuadTree::Render(ViewFrustum & viewFrustum, ID3D11DeviceContext * pDevCon)
{
	m_drawCount = 0;

	RenderNode(m_parent, viewFrustum, pDevCon);
}

void QuadTree::createTreeNode(NodeTypeQuadTree * node, int xStart, int zStart, int size, ID3D11Device * pDev)
{
	int numOfTriangles;
	vector<unsigned long> indices;

	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT hr;

	//initialize node and set it position in world
	node->xIndex = xStart;
	node->zIndex = zStart;
	node->size = size;
	node->indexBuffer = NULL;
	node->radius = node->size * deltaSpace;
	node->cubeCenterVertex.x = (node->xIndex * deltaSpace) + node->radius;
	node->cubeCenterVertex.y = 0.0f;
	node->cubeCenterVertex.z = (node->zIndex * deltaSpace) + node->radius;

	for (int i = 0; i < 4; i++)
		node->nodes[i] = NULL;

	numOfTriangles = (int)pow((size), 2) * 2;
	node->triangleCount = numOfTriangles;

	//case 1 - there are no triangles in this node
	if (numOfTriangles == 0)
		return;

	//case 2 - if there are too many triangles split it up into 4 new quads
	if (numOfTriangles > MAX_TRIANGLES)
	{
		XMFLOAT2 cube1, cube2, cube3, cube4;
		int halfSide = (int)(node->size * 0.5);

		node->nodes[0] = new NodeTypeQuadTree;
		node->nodes[1] = new NodeTypeQuadTree;
		node->nodes[2] = new NodeTypeQuadTree;
		node->nodes[3] = new NodeTypeQuadTree;

		createTreeNode(node->nodes[0], node->xIndex, node->zIndex, halfSide, pDev);
		createTreeNode(node->nodes[1], node->xIndex + halfSide, node->zIndex, halfSide, pDev);
		createTreeNode(node->nodes[2], node->xIndex, node->zIndex + halfSide, halfSide, pDev);
		createTreeNode(node->nodes[3], node->xIndex + halfSide, node->zIndex + halfSide, halfSide, pDev);

		return;
	}

	//case 3 - the right nr of triangles exist create the vertex and index buffer
	//find the vertex that equals to centXZ from the vertex vector

	int z, x, width, depth;
	x = node->xIndex;
	z = node->zIndex;
	width = x + node->size;
	depth = z + node->size;
	int offset = 6 * (m_mapSize - 1);

	for (z; z < depth; z++)
	{
		for (x; x < width; x++)
		{
			int index = x * 6 + ((offset) * z);
			indices.push_back(m_indices[index]);
			indices.push_back(m_indices[index + 1]);
			indices.push_back(m_indices[index + 2]);

			indices.push_back(m_indices[index + 3]);
			indices.push_back(m_indices[index + 4]);
			indices.push_back(m_indices[index + 5]);
		}
		x = node->xIndex;
	}

	node->triangleCount = (int)indices.size() / 3;
	// set up the description of the static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (unsigned int)indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// assign the subresource structure a pointer t o the index data
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// create index buffer
	hr = pDev->CreateBuffer(&indexBufferDesc, &indexData, &node->indexBuffer);

	//free up memory
	indices.clear();
	//done
}


void QuadTree::ReleaseNode(NodeTypeQuadTree * node)
{
	// Recursively go down the tree and release the bottom nodes first.
	for (int i = 0; i < 4; i++)
	{
		if (node->nodes[i] != NULL)
		{
			ReleaseNode(node->nodes[i]);
		}
	}

	// Release the index buffer for this node.
	if (node->indexBuffer)
	{
		node->indexBuffer->Release();
		node->indexBuffer = 0;
	}

	// Release the four child nodes.
	for (int i = 0; i < 4; i++)
	{
		if (node->nodes[i])
		{
			delete node->nodes[i];
			node->nodes[i] = 0;
		}
	}

	return;
}

void QuadTree::RenderNode(NodeTypeQuadTree * node, ViewFrustum & viewFrustum, ID3D11DeviceContext * pDevCon)
{
	bool result = false;
	int count, indexCount;

	//do frustum check on the cube
	result = viewFrustum.checkCube(node->cubeCenterVertex, node->radius);
	if (result) //if it cant be seen then none of its children can be either, stop here
		return;

	if (!node->indexBuffer) //does not have indexbuffer, go deeper
	{
		count = 0;
		for (int i = 0; i < 4; i++)
		{
			if (node->nodes[i] != NULL)
			{
				count++;
				RenderNode(node->nodes[i], viewFrustum, pDevCon);
			}
		}
		if (count != 0) //if there are children we know this parent does not have triangles to render
			return;

	}
	pDevCon->IASetIndexBuffer(node->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	indexCount = node->triangleCount * 3;

	m_drawCount += node->triangleCount;

	//set the vertex and index buffer and draw the triangles from this cube
	pDevCon->DrawIndexed(indexCount, 0, 0);
	
	return;
}

void QuadTree::calculateNormalTangentBinormal(int v1, int v2, int v3, vector<VertexTypeHeightMap>& m_vertices)
{
	XMFLOAT3 vtx1, vtx2, vtx3;
	XMFLOAT2 uv1, uv2, uv3;
	XMFLOAT3 normal;

	vtx1 = m_vertices[v1].vertex;
	vtx2 = m_vertices[v2].vertex;
	vtx3 = m_vertices[v3].vertex;

	uv1 = m_vertices[v1].tex;
	uv2 = m_vertices[v2].tex;
	uv3 = m_vertices[v3].tex;

	XMVECTOR edge1 = XMLoadFloat3(&vtx2) - XMLoadFloat3(&vtx1);
	XMVECTOR edge2 = XMLoadFloat3(&vtx3) - XMLoadFloat3(&vtx1);

	XMVECTOR n = XMVector3Normalize(XMVector3Cross(edge1, edge2));

	XMStoreFloat3(&normal, n);

	m_vertices[v1].normal = normal;
	m_vertices[v2].normal = normal;
	m_vertices[v3].normal = normal;

	//load the data into XMVECTOR's so we can use DirectXMath functions/operators for calculations
	XMVECTOR vertex0, vertex1, vertex2, tex0, tex1, tex2, normal0;
	vertex0 = XMLoadFloat3(&vtx1);
	vertex1 = XMLoadFloat3(&vtx2);
	vertex2 = XMLoadFloat3(&vtx3);
	tex0 = XMLoadFloat2(&uv1);
	tex1 = XMLoadFloat2(&uv2);
	tex2 = XMLoadFloat2(&uv3);
	normal0 = XMLoadFloat3(&normal);

	//do the math
	XMVECTOR e1 = vertex1 - vertex0;
	XMVECTOR e2 = vertex2 - vertex0;
	XMVECTOR deltaUV1 = tex1 - tex0;
	XMVECTOR deltaUV2 = tex2 - tex0;

	XMVECTOR tangent;
	XMVECTOR binormal;

	float r = (deltaUV1.m128_f32[0] * deltaUV2.m128_f32[1] - deltaUV1.m128_f32[1] * deltaUV2.m128_f32[0]);
	if (fabsf(r) < 1e-6f)
	{
		// Equal to zero (almost) means the surface lies flat on its back
		tangent = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		binormal = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		r = 1.0f / r;
		tangent = (e1 * deltaUV2.m128_f32[1] - e2 * deltaUV1.m128_f32[1]) * r;
		binormal = (e2 * deltaUV1.m128_f32[0] - e1 * deltaUV2.m128_f32[0]) * r;

		tangent = XMVector3Normalize(tangent);
		binormal = XMVector3Normalize(binormal);
	}

	// As the bitangent equals to the cross product between the normal and the tangent running along the surface, calculate it
	XMVECTOR bitangent = XMVector3Cross(normal0, tangent);

	// Since we don't know if we must negate it, compare it with our computed one above
	float crossinv = (XMVector3Dot(bitangent, binormal).m128_f32[0] < 0.0f) ? -1.0f : 1.0f;
	bitangent *= crossinv;

	XMStoreFloat3(&m_vertices[v1].tangent, tangent);
	XMStoreFloat3(&m_vertices[v2].tangent, tangent);
	XMStoreFloat3(&m_vertices[v3].tangent, tangent);

	XMStoreFloat3(&m_vertices[v1].binormal, bitangent);
	XMStoreFloat3(&m_vertices[v2].binormal, bitangent);
	XMStoreFloat3(&m_vertices[v3].binormal, bitangent);
}
