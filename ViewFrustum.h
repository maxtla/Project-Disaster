#pragma once
#ifndef VIEWFRUSTUM_H
#define VIEWFRUSTUM_H

#include <DirectXMath.h>

using namespace DirectX;

class ViewFrustum
{
public:
	ViewFrustum();
	~ViewFrustum();

	void buildFrustum(float depth, XMMATRIX view, XMMATRIX projection);

	bool checkPoint(XMFLOAT3 point);
	bool checkCube(XMFLOAT3 center, float radius);

private:
	XMVECTOR planes[6];
};

#endif // !VIEWFRUSTUM_H
