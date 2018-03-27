#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include <DirectXMath.h>
#include <d3d11.h>

using namespace DirectX;

enum LightTypes
{
	POINT_LIGHT,
	DIRECTIONAL_LIGHT
};

class Light
{
private:
	__declspec(align(16))
		struct LightType
	{
		DirectX::XMVECTOR lightPosOrDir;
		DirectX::XMVECTOR lightColor;
		DirectX::XMVECTOR ambientLight;
	};

	ID3D11Buffer * m_lightBuffer;
	int type;
	XMVECTOR focusPoint;
	XMMATRIX lightView;
	XMMATRIX lightProjection;
	void makeMatrices(LightType &light);
public:
	Light();
	~Light();
	bool initLightBuffer(ID3D11Device * pDev, int type, XMVECTOR posOrDir, XMVECTOR color, XMVECTOR ambient, XMVECTOR focusPoint);
	void Release();
	ID3D11Buffer * getLightBuffer() { return this->m_lightBuffer; }
	int getType() const { return this->type; }
	XMMATRIX getLightView() { return this->lightView; }
	XMMATRIX getLightProjection() { return this->lightProjection; }
};
#endif // !LIGHT_H

