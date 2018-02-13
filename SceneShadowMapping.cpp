#include "SceneShadowMapping.h"



SceneShadowMapping::SceneShadowMapping()
{
}


SceneShadowMapping::~SceneShadowMapping()
{
}

bool SceneShadowMapping::initScene(Application * pApp, HINSTANCE hInstance, HWND hwnd)
{
	m_depthShader = new DepthShader();
	if (!m_depthShader->Initialize(pApp->pDev, hwnd))
		return false;

	this->m_modelLoader = new ModelLoader();
	if (!this->m_modelLoader->load(pApp->pDev, "Assets//brickCube.obj", XMMatrixIdentity()))
		return false;

	XMMATRIX floor = XMMatrixIdentity();
	floor = XMMatrixMultiply(floor, XMMatrixTranslation(0.0f, -1.0f, 0.0f));

	if (!this->m_modelLoader->load(pApp->pDev, "Assets//woodPlane.obj", floor))
		return false;

	m_shadowMapShader = new ShadowMapShader();
	if (!m_shadowMapShader->Initialize(pApp->pDev, hwnd))
		return false;

	m_light = new Light();
	if (!m_light->initLightBuffer(pApp->pDev, LightTypes::POINT_LIGHT, XMVectorSet(0.0f, 3.0f, 0.0001f, 1.0f),
		XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f), XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)))
	{
		return false;
	}

	this->start_time = high_resolution_clock::now();
	this->rotationValue = 0.0f;

	return true;
}

void SceneShadowMapping::updateScene()
{
	this->current_time = high_resolution_clock::now();
	duration<double, std::milli> delta_time = this->current_time - this->start_time;
	if (delta_time.count() > (1000 / ROTATIONSPEED_NORMALMAP_RENDER_MODEL))
	{
		this->rotationValue += ROTATIONAL_INCREASE;
		this->start_time = high_resolution_clock::now();
		if (this->rotationValue * XM_PI >= 360.f)
			this->rotationValue -= 360.f;
	}

	XMMATRIX cube = XMMatrixMultiply(XMMatrixTranslation(0.0f, 1.0f, 0.5f),XMMatrixRotationY(rotationValue));
	cube = XMMatrixMultiply(cube, XMMatrixScaling(0.5f, 0.5f, 0.5f));
	this->m_modelLoader->update(cube, 0);

	XMMATRIX floor = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(0.0f, -1.0f, 0.0f));
	this->m_modelLoader->update(floor, 1);
}

void SceneShadowMapping::renderScene(Application * pApp)
{
	//reset device context to avoid weird behaviour
	pApp->pDevCon->ClearState();
	//render scene to texture for shadow mapping
	this->m_depthShader->setRenderTarget(pApp->pDevCon);

	for (int i = 0; i < m_modelLoader->size(); i++)
	{
		Model model = m_modelLoader->getModel(i);
		model.Render(pApp->pDevCon);
		this->m_depthShader->Render(pApp->pDevCon, model.getVertexCount(), model.getWorld(), m_light->getLightView(), m_light->getLightProjection());
	}
	/*pApp->pSwapChain->Present(0, 0);*/
	//reset device context to avoid weird behaviour
	pApp->pDevCon->ClearState();
	//reset render target
	pApp->pDevCon->OMSetRenderTargets(1, &pApp->pRTV, pApp->pDSV);
	//reset the viewport
	pApp->pDevCon->RSSetViewports(1, &pApp->_viewPort);
	//clear scene
	float color[4] = { 0.f, 0.f, 0.f, 1.0f };
	pApp->pDevCon->ClearRenderTargetView(pApp->pRTV, color);
	pApp->pDevCon->ClearDepthStencilView(pApp->pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	for (int i = 0; i < m_modelLoader->size(); i++)
	{
		Model model = m_modelLoader->getModel(i);
		model.Render(pApp->pDevCon);
		this->m_shadowMapShader->Render(pApp->pDevCon, model.getVertexCount(), model.getWorld(), pApp->view, pApp->projection,
			m_light->getLightView(), m_light->getLightProjection(), model.getTexture(), m_depthShader->getDepthBufferSRV());
	}

	pApp->pSwapChain->Present(0, 0);
}

void SceneShadowMapping::Release()
{
	this->m_depthShader->Release();
	delete this->m_depthShader;
	this->m_depthShader = nullptr;

	this->m_modelLoader->Release();
	delete this->m_modelLoader;
	this->m_modelLoader = nullptr;

	this->m_shadowMapShader->Release();
	delete this->m_shadowMapShader;
	this->m_shadowMapShader = nullptr;

	this->m_light->Release();
	delete this->m_light;
	this->m_light = nullptr;
}
