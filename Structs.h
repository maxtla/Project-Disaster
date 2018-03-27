#pragma once
#ifndef STRUCTS_H
#define STRUCTS_H

#include <DirectXMath.h>
using namespace DirectX;

struct VertexTypeHeightMap
{
	XMFLOAT3 vertex;
	XMFLOAT2 tex;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 binormal;
};



#endif // !STRUCTS_H

