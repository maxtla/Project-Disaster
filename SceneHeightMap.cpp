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
	m_heightMap->Render(pApp->pDevCon, XMMatrixIdentity(), pApp->view, pApp->projection);

	//text output
	pApp->m_spriteBatch->Begin();
	wstring output = L"Triangles drawn: ";
	int triangleCount = m_heightMap->getNrOfTriangles();
	output += to_wstring(triangleCount);
	XMVECTOR origin = pApp->m_font->MeasureString(output.c_str()) / 2.f;
	pApp->m_font->DrawString(pApp->m_spriteBatch.get(), output.c_str(), XMLoadFloat2(&pApp->m_fontPos), Colors::White, 0.f, origin);
	pApp->m_spriteBatch->End();

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
