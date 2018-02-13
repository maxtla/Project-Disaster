#pragma once
#ifndef _SCENEDEFERREDRENDERING_H
#define _SCENEDEFERREDRENDERING_H

#include "DeferredBuffer.h"
#include "DeferredShader.h"
#include "LightShader.h"
#include "ModelLoader.h"
#include "Application.h"

#define ROTATIONSPEED_DEFERRED_RENDER_MODEL 100
#define ROTATIONAL_INCREASE 0.0005f

class Application;

class SceneDeferredRendering 
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

#endif // !_SCENEDEFERREDRENDERING_H
