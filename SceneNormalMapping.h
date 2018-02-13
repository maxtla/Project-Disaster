#pragma once
#ifndef _SCENENORMALMAPPING_H
#define _SCENENORMALMAPPING_H

#include "NormalMapShader.h"
#include "ModelLoader.h"
#include "Application.h"
#include <chrono>

using namespace DirectX;
class Application;
class NormalMapShader;

#define ROTATIONSPEED_NORMALMAP_RENDER_MODEL 100
#define ROTATIONAL_INCREASE 0.0005f

class SceneNormalMapping
{
private:
	__declspec(align(16))
	struct LightBufferType
	{
		DirectX::XMVECTOR lightPos;
		DirectX::XMVECTOR lightColor;
		DirectX::XMVECTOR ambientLight;
	};

	NormalMapShader* m_normalMapShader;
	ModelLoader * m_modelLoader;
	ID3D11Buffer* m_lightBuffer;
	LightBufferType m_lightBufferData;
	high_resolution_clock::time_point start_time, current_time;
	float rotationValue;

	bool initBuffer(ID3D11Device* pDev);
public:
	SceneNormalMapping();
	~SceneNormalMapping();
	bool initScene(Application * pApp, HINSTANCE hInstance, HWND hwnd);
	void updateScene();
	void renderScene(Application * pApp);
	void Release();
};
#endif // !_SCENENORMALMAPPING_H

