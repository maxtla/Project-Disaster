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
		if (!this->m_heightMap->initialize(pApp, hwnd, (int)pow(2, 7) + 1, 0.3f))
			return false;
	}
	return true;
}

void SceneHeightMap::updateScene()
{
	this->m_heightMap->update();
}

void SceneHeightMap::renderScene(Application * pApp)
{
	//clear states
	pApp->pDevCon->ClearState();
	pApp->pDevCon->OMSetRenderTargets(1, &pApp->pRTV, pApp->pDSV);
	pApp->pDevCon->RSSetState(pApp->pRasterState);

	float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	pApp->pDevCon->ClearRenderTargetView(pApp->pRTV, color);
	pApp->pDevCon->ClearDepthStencilView(pApp->pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//render map
	m_heightMap->Render(pApp, XMMatrixIdentity(), pApp->view, pApp->projection);

	//text output
	pApp->textToScreen(L"Scene 4", XMFLOAT2(50.f, 50.f), XMFLOAT2(0.5f, 0.5f));
	pApp->textToScreen(L"Triangles: " + to_wstring(m_heightMap->getNrOfTriangles()), XMFLOAT2(50.f, 100.f), XMFLOAT2(0.45f, 0.45f));
	pApp->textToScreen(L"(Y) Toggle follow", XMFLOAT2(50.f, 130.f), XMFLOAT2(0.4f, 0.4f));
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