#pragma once
#ifndef _SCENEINTERFACE_H
#define _SCENEINTERFACE_H
#include <d3d11.h>
#include <DirectXMath.h>
#include "Application.h"
class Application;
class SceneInterface
{
public:
	virtual ~SceneInterface() {};
	virtual bool initScene(Application * pApp, HINSTANCE hInstance, HWND hwnd) = 0;
	virtual void updateScene() = 0;
	virtual void renderScene(Application * pApp) = 0;
	virtual void Release() = 0;
};
#endif // !_SCENEINTERFACE_H

