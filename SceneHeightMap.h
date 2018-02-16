#pragma once
#ifndef SCENEHEIGHTMAP_H
#define SCENEHEIGHTMAP_H

#include "HeightMap.h"
#include "Application.h"

using namespace DirectX;

class SceneHeightMap
{
public:
	SceneHeightMap();
	~SceneHeightMap();
	bool initScene(Application * pApp, HINSTANCE hInstance, HWND hwnd);
	void updateScene();
	void renderScene(Application * pApp);
	void Release();

private:
	HeightMap * m_heightMap;
};
#endif // !SCENEHEIGHTMAP_H

