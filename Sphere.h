#pragma once
#ifndef SPHERE_H
#define SPHERE_H

#include "ModelLoader.h"
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <d3d11.h>
#include <vector>
#include "Structs.h"
#include "Movement.h"

using namespace DirectX;
using namespace std;

class Sphere
{
public:
	Sphere();
	~Sphere();
	bool initialize(ID3D11Device* pDev, Movement *inputHandler);
	void Release();
	void Render(ID3D11DeviceContext* pDevCon);
	bool intersectBoundingBox(XMFLOAT3 center, float radius, vector<int> indices);
	void update(vector<VertexTypeHeightMap> vertices);
	void move();
	XMMATRIX getWorld();
private:
	ModelLoader * m_modelLoader;
	BoundingSphere * m_sphereBounds;
	vector<vector<int>> m_indexBufferList;
	Movement * inputHandler;
	
};
#endif // !SPHERE_H

