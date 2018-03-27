#include "SceneMousePicking.h"

SceneMousePicking::SceneMousePicking()
{
	this->m_mousePickingShader = nullptr;
	this->m_cubes = nullptr;
}

SceneMousePicking::~SceneMousePicking()
{
}

bool SceneMousePicking::initScene(Application* pApp, HINSTANCE hInstance, HWND hwnd)
{
	this->m_mousePickingShader = new MousePickingShader();
	this->m_cubes = new Cube();
	ID3D11Buffer* cubeBuffer;
	this->nrOfCubes = 2;
	this->m_cubes->initialize(pApp, &cubeBuffer, this->nrOfCubes);
	this->m_mousePicking.initialize(pApp->_viewPort.Width, pApp->_viewPort.Height);
	if (!this->m_mousePickingShader->initialize(pApp->pDev, hwnd, cubeBuffer))
		return false;

	return true;
}

void SceneMousePicking::updateScene(Application* pApp)
{
	if (pApp->inputHandler->getShootRay())
	{
		DirectX::XMVECTOR ray;
		ray = this->m_mousePicking.detectInput(pApp->inputHandler->getMouseCoords(), pApp->inputHandler->getCamPos(), pApp->projection, pApp->view, DirectX::XMMatrixIdentity());
		this->m_cubes->intersectBoundingBox(pApp->inputHandler->getCamPos(), ray);
		pApp->inputHandler->setShootRay(false);
	}

	int lockedBoxId = this->m_cubes->getHitBoxId();
	bool isBoxHit = this->m_cubes->getBoxHit();
	this->current_time = high_resolution_clock::now();
	duration<double, std::milli> delta_time = this->current_time - this->start_time;
	if (isBoxHit)
	{
		this->m_cubes->update(pApp, this->m_mousePickingShader->getVertexBuffer(), isBoxHit);
		this->start_time = high_resolution_clock::now();
	}
	else if (!isBoxHit && delta_time.count() > 1000.0)
	{
		this->m_cubes->update(pApp, this->m_mousePickingShader->getVertexBuffer(), isBoxHit);
	}
}

void SceneMousePicking::renderScene(Application* pApp)
{
	//reset device context to avoid weird behaviour
	pApp->pDevCon->ClearState();
	//set render target
	pApp->pDevCon->OMSetRenderTargets(1, &pApp->pRTV, pApp->pDSV);
	//set the viewport
	pApp->pDevCon->RSSetViewports(1, &pApp->_viewPort);
	//clear scene
	float color[4] = { 0.f, 0.f, 0.f, 1.0f };
	pApp->pDevCon->ClearRenderTargetView(pApp->pRTV, color);
	pApp->pDevCon->ClearDepthStencilView(pApp->pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	this->m_mousePickingShader->render(pApp->pDevCon, this->m_cubes->getStride(), DirectX::XMMatrixIdentity(), pApp->view, pApp->projection);
	
	pApp->pSwapChain->Present(0, 0);
}

void SceneMousePicking::Release()
{
	this->m_mousePickingShader->release();
	delete this->m_mousePickingShader;
	this->m_mousePickingShader = nullptr;

	this->m_cubes->Release();
	delete this->m_cubes;
	this->m_cubes = nullptr;
}