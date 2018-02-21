#include "ViewFrustum.h"



ViewFrustum::ViewFrustum()
{
	for (int i = 0; i < 6; i++)
		planes[i] = g_XMZero;
}


ViewFrustum::~ViewFrustum()
{
}

void ViewFrustum::buildFrustum(float depth, XMMATRIX view, XMMATRIX projection)
{
	float zMin, r;
	XMMATRIX matrix;
	XMFLOAT4X4 _view, _proj, _m;
	XMStoreFloat4x4(&_view, view);
	XMStoreFloat4x4(&_proj, projection);

	//calculate the min Z distance in the frustum
	zMin = -_proj._43 / _proj._33;
	r = depth / (depth - zMin);
	_proj._33 = r;
	_proj._43 = -r * zMin;

	//create the frustum matrix
	projection = XMLoadFloat4x4(&_proj);
	matrix = XMMatrixMultiply(view, projection);
	XMStoreFloat4x4(&_m, matrix);

	//calculate and store the planes
	XMFLOAT4 plane;

	//near plane
	plane.x = _m._14 + _m._13;
	plane.y = _m._24 + _m._23;
	plane.z = _m._34 + _m._33;
	plane.w = _m._44 + _m._43;
	planes[0] = XMLoadFloat4(&plane);

	//far plane
	plane.x = _m._14 - _m._13;
	plane.y = _m._24 - _m._23;
	plane.z = _m._34 - _m._33;
	plane.w = _m._44 - _m._43;
	planes[1] = XMLoadFloat4(&plane);

	//left plane
	plane.x = _m._14 + _m._11;
	plane.y = _m._24 + _m._21;
	plane.z = _m._34 + _m._31;
	plane.w = _m._44 + _m._41;
	planes[2] = XMLoadFloat4(&plane);

	//right plane
	plane.x = _m._14 - _m._11;
	plane.y = _m._24 - _m._21;
	plane.z = _m._34 - _m._31;
	plane.w = _m._44 - _m._41;
	planes[3] = XMLoadFloat4(&plane);

	//top plane
	plane.x = _m._14 - _m._12;
	plane.y = _m._24 - _m._22;
	plane.z = _m._34 - _m._32;
	plane.w = _m._44 - _m._42;
	planes[4] = XMLoadFloat4(&plane);

	//bottom plane
	plane.x = _m._14 + _m._12;
	plane.y = _m._24 + _m._22;
	plane.z = _m._34 + _m._32;
	plane.w = _m._44 + _m._42;
	planes[5] = XMLoadFloat4(&plane);

	//normalize the planes
	for (int i = 0; i < 6; i++)
	{
		planes[i] = XMPlaneNormalize(planes[i]);
	}
	//done
}

bool ViewFrustum::checkPoint(XMFLOAT3 point)
{
	//check if the point is inside all the six planes
	for (int i = 0; i < 6; i++)
	{
		if (XMPlaneDotCoord(planes[i], XMLoadFloat3(&point)).m128_f32[0] < 0.0f)
		{
			return false; //the point is outside the view frustum
		}
	}
	return true;
}

bool ViewFrustum::checkCube(XMFLOAT3 center, float radius)
{
	for (int i = 0; i < 6; i++)
	{
		if (XMPlaneDotCoord(planes[i], XMVectorSet((center.x - radius), (center.y - radius), (center.z - radius), 0.0f)).m128_f32[0] >= 0.0f)
		{
			continue; //this point is inside
		}

		if (XMPlaneDotCoord(planes[i], XMVectorSet((center.x + radius), (center.y - radius), (center.z - radius), 0.0f)).m128_f32[0] >= 0.0f)
		{
			continue; //this point is inside
		}

		if (XMPlaneDotCoord(planes[i], XMVectorSet((center.x - radius), (center.y + radius), (center.z - radius), 0.0f)).m128_f32[0] >= 0.0f)
		{
			continue; //this point is inside
		}

		if (XMPlaneDotCoord(planes[i], XMVectorSet((center.x + radius), (center.y + radius), (center.z - radius), 0.0f)).m128_f32[0] >= 0.0f)
		{
			continue; //this point is inside
		}

		if (XMPlaneDotCoord(planes[i], XMVectorSet((center.x - radius), (center.y - radius), (center.z + radius), 0.0f)).m128_f32[0] >= 0.0f)
		{
			continue; //this point is inside
		}

		if (XMPlaneDotCoord(planes[i], XMVectorSet((center.x + radius), (center.y - radius), (center.z + radius), 0.0f)).m128_f32[0] >= 0.0f)
		{
			continue; //this point is inside
		}

		if (XMPlaneDotCoord(planes[i], XMVectorSet((center.x - radius), (center.y + radius), (center.z + radius), 0.0f)).m128_f32[0] >= 0.0f)
		{
			continue; //this point is inside
		}

		if (XMPlaneDotCoord(planes[i], XMVectorSet((center.x + radius), (center.y + radius), (center.z + radius), 0.0f)).m128_f32[0] >= 0.0f)
		{
			continue; //this point is inside
		}
		return true; //if we get to this point we know its outside the frustum and can be culled
	}
	return false;
}
