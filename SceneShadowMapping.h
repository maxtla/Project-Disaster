#pragma once
#ifndef _SCENESHADOWMAPPING_H
#define _SCENESHADOWMAPPING_H

#include "ModelLoader.h"
#include "Application.h"
#include "Light.h"
#include "DepthShader.h"
#include "ShadowMapShader.h"

using namespace DirectX;
class Application;

#define ROTATIONSPEED_SHADOWMAP_CUBE_MODEL 100;
#define ROTATIONAL_INCREASE_SHADOWMAP 0.005f;

class SceneShadowMapping
{
private:
	DepthShader * m_depthShader;
	ShadowMapShader * m_shadowMapShader;
	ModelLoader * m_modelLoader;
	Light * m_light;
	high_resolution_clock::time_point start_time, current_time;
	float rotationValue;
public:
	SceneShadowMapping();
	~SceneShadowMapping();
	bool initScene(Application * pApp, HINSTANCE hInstance, HWND hwnd);
	void updateScene();
	void renderScene(Application * pApp);
	void Release();
};
#endif // !_SCENESHADOWMAPPING_H

