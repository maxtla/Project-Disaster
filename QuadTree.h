#pragma once
#ifndef QUADTREE_H
#define QUADTREE_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include "ViewFrustum.h"
#include "Sphere.h"
#include "Structs.h"

using namespace DirectX;
using namespace std;

#define MAX_TRIANGLES 1000
class Sphere;

class QuadTree
{
public:

	QuadTree();
	~QuadTree();

	bool initialize(vector<unsigned int> indices, int mapSize, float delta, ID3D11Device * pDev);
	void Release();
	void Render(ViewFrustum &viewFrustum, ID3D11DeviceContext * pDevCon);
	int getTriangleCount() const { return m_drawCount; }
	void intersectionTestSphere(Sphere &sphere);


private:
	struct NodeTypeQuadTree
	{
		int xIndex;
		int zIndex;
		int size;
		float radius;
		int triangleCount;
		XMFLOAT3 cubeCenterVertex;
		ID3D11Buffer* indexBuffer;
		vector<int> indices;
		NodeTypeQuadTree* nodes[4];
	};
	//methods
	void createTreeNode(NodeTypeQuadTree *node, int XstartIndex, int zStartIndex, int size, ID3D11Device* pDev);
	void ReleaseNode(NodeTypeQuadTree *node);
	void RenderNode(NodeTypeQuadTree *node, ViewFrustum &viewFrustum,ID3D11DeviceContext *pDevCon);
	void calculateNormalTangentBinormal(int v1, int v2, int v3, vector<VertexTypeHeightMap> &m_vertices);
	void c_intersectionTestSphere(Sphere &sphere, NodeTypeQuadTree * node);
	//members
	NodeTypeQuadTree * m_parent;
	int m_mapSize;
	int m_triangleCount;
	int m_drawCount;
	float deltaSpace; //the space that seperates the vertices, need this construct the center vertex of each cube
	vector<unsigned int> m_indices;

};
#endif // !QUADTREE_H

