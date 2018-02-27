#ifndef SCENEBILLBOARDING_H
#define SCENEBILLBOARDING_H

#include"ModelLoader.h"
#include"Application.h"
#include"Billboarding.h"

using namespace DirectX;
class Application;

class SceneBillboarding
{
private:
	Billboarding * m_billBoard;
	ModelLoader* m_modelLoader;
	float fallingSpeed;
	high_resolution_clock::time_point start_time, current_time;
public:
	SceneBillboarding();
	~SceneBillboarding();
	bool initScene(Application* pApp, HINSTANCE hInstance, HWND hwnd);
	void updateScene();
	void renderScene(Application* pApp);
	void Release();

};

#endif // !SCENEBILLBOARDING_H

