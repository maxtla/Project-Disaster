#ifndef SCENEMOUSEPICKING_H
#define SCENEMOUSEPICKING_H

#include "Application.h"
#include "MousePicking.h"
#include "MousePickingShader.h"
#include "Cube.h"

class Application;
class MousePicking;
class MousePickingShader;
class Cube;

class SceneMousePicking
{
private:
	MousePicking m_mousePicking;
	MousePickingShader* m_mousePickingShader;
	Cube* m_cubes;
	int nrOfCubes;
	high_resolution_clock::time_point start_time, current_time;

public:
	SceneMousePicking();
	virtual~SceneMousePicking();
	bool initScene(Application* pApp, HINSTANCE hInstance, HWND hwnd);
	void updateScene(Application* pApp);
	void renderScene(Application* pApp);
	void Release();
};

#endif