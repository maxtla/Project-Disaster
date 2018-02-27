#pragma once
#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "Movement.h"
//scenes
#include "SceneDeferredRendering.h"
#include "SceneNormalMapping.h"
#include "SceneShadowMapping.h"
#include "SceneHeightMap.h"
#include"SceneBillboarding.h"
//directxtk 
#include "SpriteFont.h"
#include "SpriteBatch.h"

//collect comments for the linker to include libraries here
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

//collect namespace:s here
using namespace DirectX;
using namespace std;

//collect macros here
#define WIDTH 1280
#define HEIGHT 720


//if you add a new scene you have made it is very important to forward declare 
//to do this just write: class SceneClassName;
class SceneNormalMapping;
class SceneDeferredRendering;
class SceneShadowMapping;
class SceneHeightMap;
class SceneBillboarding;

class Application
{
public:
	Application();
	~Application();
	bool initApplication(HINSTANCE, HWND);
	bool initScenes(HINSTANCE, HWND);
	void handleInput();
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
	ID3D11RasterizerState* pRasterStateNoCulling;
	//matrices
	XMMATRIX view;
	XMMATRIX projection;
	//fonts
	std::unique_ptr<DirectX::SpriteFont> m_font;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	XMFLOAT2 m_fontPos;
	void textToScreen(wstring text, XMFLOAT2 position, XMFLOAT2 scaling);
	void camInfoToScreen(XMFLOAT2 position, XMFLOAT2 scaling);
	Movement * inputHandler;
private:
	//Scene pointers
	SceneShadowMapping * pSceneShadowMap;
	SceneNormalMapping * pSceneNormalMap;
	SceneDeferredRendering * pSceneDefRender;
	SceneHeightMap * pSceneHeightMap;
	SceneBillboarding * pSceneBillboarding;
	int currentScene;
};
#endif // !_APPLICATION_H

