#include "SceneHeightMap.h"



SceneHeightMap::SceneHeightMap()
{
	this->m_heightMap = nullptr;
}


SceneHeightMap::~SceneHeightMap()
{
}

bool SceneHeightMap::initScene(Application * pApp, HINSTANCE hInstance, HWND hwnd)
{
	if (m_heightMap == nullptr)
	{
		this->m_heightMap = new HeightMap();
		if (!this->m_heightMap->initialize(pApp->pDev, hwnd, (int)pow(2, 9) + 1, 0.5f))
			return false;
	}
	return true;
}

void SceneHeightMap::updateScene()
{
	
}

void SceneHeightMap::renderScene(Application * pApp)
{
	//clear states
	pApp->pDevCon->ClearState();
	pApp->pDevCon->OMSetRenderTargets(1, &pApp->pRTV, pApp->pDSV);

	float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	pApp->pDevCon->ClearRenderTargetView(pApp->pRTV, color);
	pApp->pDevCon->ClearDepthStencilView(pApp->pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//render map
	m_heightMap->Render(pApp, XMMatrixIdentity(), pApp->view, pApp->projection);

	//text output
	pApp->textToScreen(L"Scene 4", XMFLOAT2(50.f, 50.f), XMFLOAT2(0.5f, 0.5f));
	pApp->textToScreen(L"Triangles: " + to_wstring(m_heightMap->getNrOfTriangles()), XMFLOAT2(50.f, 100.f), XMFLOAT2(0.45f, 0.45f));
	pApp->camInfoToScreen(XMFLOAT2(50.f, 600.f), XMFLOAT2(0.4f, 0.4f));

	//present the final frame
	pApp->pSwapChain->Present(0, 0);
}

void SceneHeightMap::Release()
{
	if (this->m_heightMap)
	{
		m_heightMap->Release();
		delete m_heightMap;
		m_heightMap = nullptr;
	}
}