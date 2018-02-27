#include "SceneBillboarding.h"

SceneBillboarding::SceneBillboarding()
{
}

SceneBillboarding::~SceneBillboarding()
{
}

bool SceneBillboarding::initScene(Application * pApp, HINSTANCE hInstance, HWND hwnd)
{
	m_modelLoader = new ModelLoader();
	if (!this->m_modelLoader->load(pApp->pDev, "Assets//brickCube.obj", XMMatrixIdentity()))
		return false;

	XMMATRIX floor = XMMatrixIdentity();
	floor = XMMatrixMultiply(floor, XMMatrixTranslation(0.0f, -1.0f, 0.0f));

	if (!this->m_modelLoader->load(pApp->pDev, "Assets//woodPlane.obj", floor))
		return false;
	
	m_billBoard = new Billboarding();
	if (!this->m_billBoard->initialize(pApp->pDev, hwnd))
		return false;

	this->start_time = high_resolution_clock::now();
	this->fallingSpeed = 0.0f;

	return true;
}

void SceneBillboarding::updateScene()
{
	this->current_time = high_resolution_clock::now();
	duration<double, std::milli> delta_time = this->current_time - this->start_time;
	
	XMMATRIX billboard = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(0.0f, 1.0f, 0.5f));

	billboard = XMMatrixMultiply(billboard, XMMatrixScaling(0.5f, 0.5f, 0.5f));
	this->m_modelLoader->update(billboard, 0);


	XMMATRIX floor = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(0.0f, -1.0f, 0.0f));
	this->m_modelLoader->update(floor, 1);

}

void SceneBillboarding::renderScene(Application* pApp)
{

	pApp->pDevCon->ClearState();
	//set render target
	
	pApp->pDevCon->OMSetRenderTargets(1, &pApp->pRTV, pApp->pDSV);
	//set the viewport
	pApp->pDevCon->RSSetViewports(1, &pApp->_viewPort);
	pApp->pDevCon->RSSetState(pApp->pRasterState);
	//clear scene
	float color[4] = { 0.f,0.f,0.f,1.0f };
	pApp->pDevCon->ClearRenderTargetView(pApp->pRTV, color);
	pApp->pDevCon->ClearDepthStencilView(pApp->pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	for (int i = 0; i < m_modelLoader->size(); i++)
	{
		Model model = m_modelLoader->getModel(i);
		model.Render(pApp->pDevCon);
		this->m_billBoard->render(pApp->pDevCon, model.getVertexCount(), model.getWorld(), pApp->view
			, pApp->projection, model.getTexture(), model.getSampler(), XMVectorSet(0.0f, 2.0f, -3.0f, 1.0f));
	}
	pApp->textToScreen(L"Scene 5", XMFLOAT2(50.f, 50.f), XMFLOAT2(0.5f, 0.5f));
	pApp->camInfoToScreen(XMFLOAT2(50.f, 600.f), XMFLOAT2(0.4f, 0.4f));

	pApp->pSwapChain->Present(0, 0);
}

void SceneBillboarding::Release()
{
	this->m_billBoard->Release();
	delete this->m_billBoard;
	this->m_billBoard = nullptr;

	this->m_modelLoader->Release();
	delete this->m_modelLoader;
	this->m_modelLoader = nullptr;
}
