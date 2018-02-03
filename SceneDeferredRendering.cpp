#include "SceneDeferredRendering.h"



SceneDeferredRendering::SceneDeferredRendering()
{
	this->pDefBuff = nullptr;
	this->pDefShader = nullptr;
	this->pLightShader = nullptr;
	this->pModelLoader = nullptr;
}


SceneDeferredRendering::~SceneDeferredRendering()
{
}

bool SceneDeferredRendering::initScene(Application * pApp, HINSTANCE hInstance, HWND hwnd)
{
	//here we have to create objects, load in shaders, set viewport, load models, textures, sounds, etc.
	XMMATRIX m_world = XMMatrixIdentity();
	if (this->pModelLoader == nullptr)
		this->pModelLoader = new ModelLoader();
	//load in Cube.obj

	if (!this->pModelLoader->load(pApp->pDev, "Assets//SoilCube.obj", XMMatrixMultiply(m_world, XMMatrixTranslation(0.0f, 2.0f, 0.0f))))
		return false;
	m_world = XMMatrixMultiply(m_world, XMMatrixTranslation(0.0f, -1.0f, 0.0f));
	if (!pModelLoader->load(pApp->pDev, "Assets//woodPlane.obj", m_world))
		return false;

	//init deferred buffer
	if (this->pDefBuff == nullptr)
		this->pDefBuff = new DeferredBuffer();

	if (!this->pDefBuff->initialize(pApp->pDev, WIDTH, HEIGHT, 1, 0))
		return false;
	//init deferred shader
	if (this->pDefShader == nullptr)
		this->pDefShader = new DeferredShader();

	if (!this->pDefShader->initialize(pApp->pDev, hwnd))
		return false;
	//init light shader
	if (this->pLightShader == nullptr)
		this->pLightShader = new LightShader();

	if (!this->pLightShader->initialize(pApp->pDev, hwnd))
		return false;

	this->rotationValue = 0.0f;
	this->start_time = high_resolution_clock::now();

	return true;
}

void SceneDeferredRendering::updateScene()
{
	this->current_time = high_resolution_clock::now();
	duration<double, std::milli> delta_time = this->current_time - this->start_time;
	if (delta_time.count() > (1000 / ROTATIONSPEED_DEFERRED_RENDER_MODEL))
	{
		this->rotationValue += ROTATIONAL_INCREASE;
		this->start_time = high_resolution_clock::now();
		if (this->rotationValue * XM_PI >= 360.f)
			this->rotationValue -= 360.f;
	}

	for (int i = 0; i < this->pModelLoader->size(); i++)
	{
		if (i == 0)
		{
			//rotate monkey
			XMMATRIX monkey = XMMatrixMultiply(XMMatrixTranslation(0.0f, 0.5f, 0.0f), XMMatrixRotationY(rotationValue));
			this->pModelLoader->update(monkey, i);
		}
		else
		{
			XMMATRIX floor = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(0.0f, -1.0f, 0.0f));
			this->pModelLoader->update(floor, i);
		}
	}
	

}

void SceneDeferredRendering::renderScene(Application * pApp)
{
	// ------ FIRST PASS -----
	//first we must render the scene using the deferred shader
	this->pDefBuff->setRenderTargets(pApp->pDevCon);
	this->pDefBuff->clearRenderTargets(pApp->pDevCon, 0.0f, 0.0f, 0.0f, 1.0f);
	for (int i = 0; i < this->pModelLoader->size(); i++)
	{
		pModelLoader->getModel(i).Render(pApp->pDevCon);
		pDefShader->render(pApp->pDevCon, pModelLoader->getModel(i).getVertexCount(), pModelLoader->getModel(i).getWorld(),
			pApp->view, pApp->projection, pModelLoader->getModel(i).getTexture(), pModelLoader->getModel(i).getSampler());
	}



	// ------ LIGHT PASS -----
	//reset the render target back to the original backbuffer
	pApp->pDevCon->OMSetRenderTargets(1, &pApp->pRTV, pApp->pDSV);
	//reset the viewport
	pApp->pDevCon->RSSetViewports(1, &pApp->_viewPort);
	//clear scene for 2D rendering
	float color[4] = { 0.f, 0.f, 0.f, 1.0f };
	pApp->pDevCon->ClearRenderTargetView(pApp->pRTV, color);
	pApp->pDevCon->ClearDepthStencilView(pApp->pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	//setup scene for 2D rendering - disable depth
	pApp->pDevCon->OMSetDepthStencilState(pApp->pDepthDisabledStencilState, 1);

	for (int i = 0; i < pModelLoader->size(); i++)
	{
		pLightShader->Render(pApp->pDevCon, XMMatrixIdentity(), pApp->view, pApp->projection, pDefBuff->getShaderResourceView(0),
			pDefBuff->getShaderResourceView(1), pDefBuff->getShaderResourceView(2),
			pModelLoader->getModel(i).getMaterialBuffer(), XMVectorSet(0.0f, 2.0f, -3.0f, 1.0f));
	}

	//enable depth
	pApp->pDevCon->OMSetDepthStencilState(pApp->pDepthStencilState, 1);

	pApp->pSwapChain->Present(0, 0);
}

void SceneDeferredRendering::Release()
{
	if (this->pDefBuff)
		this->pDefBuff->Release();
	if (this->pDefShader)
		this->pDefShader->Release();
	if (this->pLightShader)
		this->pLightShader->Release();
	if (this->pModelLoader)
		this->pModelLoader->Release();
}
