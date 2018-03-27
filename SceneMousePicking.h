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
	struct Timers
	{
		int index;
		high_resolution_clock::time_point start_time;
		high_resolution_clock::time_point current_time;
	};

	MousePicking m_mousePicking;
	MousePickingShader* m_mousePickingShader;
	Cube* m_cubes;
	int nrOfCubes;
	high_resolution_clock::time_point current_time;
	std::vector<Timers> tracker;

public:
	SceneMousePicking();
	virtual~SceneMousePicking();
	bool initScene(Application* pApp, HINSTANCE hInstance, HWND hwnd);
	void updateScene(Application* pApp);
	void renderScene(Application* pApp);
	void Release();
};

#endif