#pragma once
#include "SceneInterface.h"
#include "DeferredBuffer.h"
#include "DeferredShader.h"
#include "LightShader.h"
#include "ModelLoader.h"

#define ROTATIONSPEED_DEFERRED_RENDER_MODEL 100
#define ROTATIONAL_INCREASE 0.0005f

class SceneDeferredRendering :
	public SceneInterface
{
public:
	SceneDeferredRendering();
	~SceneDeferredRendering();
	bool initScene(Application * pApp, HINSTANCE hInstance, HWND hwnd);
	void updateScene();
	void renderScene(Application * pApp);
	void Release();
private:
	DeferredBuffer * pDefBuff;
	DeferredShader * pDefShader;
	LightShader * pLightShader;
	ModelLoader * pModelLoader;
	float rotationValue;
	high_resolution_clock::time_point start_time, current_time;

};

