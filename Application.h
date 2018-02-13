#pragma once
#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "Movement.h"
#include <time.h>
#include "SceneDeferredRendering.h"
#include "SceneNormalMapping.h"
#include "SceneShadowMapping.h"

//collect comments for the linker to include libraries here
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

enum Scenes 
{
	SceneOne,
	SceneTwo,
	SceneThree,
};
//Scenes
// SceneOne = Deferred Rendering
// SceneTwo = Normal Mapping
#define NUMOFSCENES 3;

//collect namespace:s here
using namespace DirectX;
using namespace std;

//collect macros here
#define WIDTH 1280
#define HEIGHT 720

//if you add a new scene you have made it is very important to forward declare 
//the class so the compiler knows we are using an interafce, to do this just write: class SceneClassName;
class SceneNormalMapping;
class SceneDeferredRendering;
class SceneShadowMapping;

class Application
{
public:
	Application();
	~Application();
	bool initApplication(HINSTANCE, HWND);
	bool initScenes(HINSTANCE, HWND);
	void update();
	void render();
	void Release();
	//D3D objects/pointers
	IDXGISwapChain* pSwapChain;
	ID3D11Device* pDev;
	ID3D11DeviceContext* pDevCon;
	ID3D11RenderTargetView* pRTV;
	ID3D11DepthStencilView* pDSV;
	ID3D11Texture2D* pDSBuffer;
	D3D11_VIEWPORT _viewPort;
	ID3D11DepthStencilState* pDepthStencilState;
	ID3D11DepthStencilState* pDepthDisabledStencilState;
	ID3D11RasterizerState* pRasterState;
	void switchScene();
	//matrices
	XMMATRIX view;
	XMMATRIX projection;
private:
	Movement * inputHandler;
	//Scene pointers
	SceneShadowMapping * pSceneShadowMap;
	SceneNormalMapping * pSceneNormalMap;
	SceneDeferredRendering * pSceneDefRender;
	int currentScene;
	clock_t start_time;
};
#endif // !_APPLICATION_H

