#include "SceneNormalMapping.h"



bool SceneNormalMapping::initBuffer(ID3D11Device * pDev)
{
	this->m_lightBufferData.lightPos = XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f);
	this->m_lightBufferData.lightColor = XMVectorSet(0.0f, 1.0f, 1.0f, 1.0f);
	this->m_lightBufferData.ambientLight = XMVectorSet(0.0f, 1.0f, 1.0f, 1.0f);

	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_SUBRESOURCE_DATA lightData;

	ZeroMemory(&lightBufferDesc, sizeof(D3D11_BUFFER_DESC));

	lightBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	lightBufferDesc.ByteWidth = (unsigned int)(sizeof(LightBufferType));
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = 0;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	lightData.pSysMem = &m_lightBufferData;
	lightData.SysMemPitch = 0;
	lightData.SysMemSlicePitch = 0;

	HRESULT hr = pDev->CreateBuffer(&lightBufferDesc, &lightData, &this->m_lightBuffer);
	if (FAILED(hr))
	{
		/*const TCHAR * err = _com_error(hr).ErrorMessage();

		OutputDebugString(err);*/
		return false;
	}

	return true;
}

SceneNormalMapping::SceneNormalMapping()
{
	m_normalMapShader = nullptr;
	m_modelLoader = nullptr;
	m_lightBuffer = nullptr;
}


SceneNormalMapping::~SceneNormalMapping()
{
}

bool SceneNormalMapping::initScene(Application * pApp, HINSTANCE hInstance, HWND hwnd)
{
	m_normalMapShader = new NormalMapShader();
	if (!m_normalMapShader->initialize(pApp->pDev, hwnd))
		return false;

	m_modelLoader = new ModelLoader();
	if (!m_modelLoader->load(pApp->pDev, "Assets//brickCylinder.obj", XMMatrixIdentity()))
		return false;

	if (!initBuffer(pApp->pDev))
		return false;

	this->start_time = high_resolution_clock::now();
	this->rotationValue = 0.0f;

	return true;
}

void SceneNormalMapping::updateScene()
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

	XMMATRIX world = XMMatrixMultiply(XMMatrixTranslation(0.0f, 0.0f, 0.0f),XMMatrixRotationY(rotationValue));
	world = XMMatrixMultiply(world, XMMatrixRotationX(rotationValue - 0.33f*ROTATIONAL_INCREASE));
	this->m_modelLoader->update(world, 0);
}

void SceneNormalMapping::renderScene(Application * pApp)
{
	//reset device context to avoid weird behaviour
	pApp->pDevCon->ClearState();
	//set render target
	pApp->pDevCon->OMSetRenderTargets(1, &pApp->pRTV, pApp->pDSV);
	//set the viewport
	pApp->pDevCon->RSSetViewports(1, &pApp->_viewPort);
	pApp->pDevCon->RSSetState(pApp->pRasterState);
	//clear scene
	float color[4] = { 0.f, 0.f, 0.f, 1.0f };
	pApp->pDevCon->ClearRenderTargetView(pApp->pRTV, color);
	pApp->pDevCon->ClearDepthStencilView(pApp->pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	for (int i = 0; i < m_modelLoader->size(); i++)
	{
		Model model = m_modelLoader->getModel(i);
		model.Render(pApp->pDevCon);
		this->m_normalMapShader->render(pApp->pDevCon, model.getVertexCount(), model.getWorld(), pApp->view,
			pApp->projection, model.getTexture(), model.getNormalMap(), true, model.getSampler(), model.getMaterialBuffer(), this->m_lightBuffer);
	}

	//text output
	pApp->textToScreen(L"Scene 2", XMFLOAT2(50.f, 50.f), XMFLOAT2(0.5f, 0.5f));
	pApp->camInfoToScreen(XMFLOAT2(50.f, 600.f), XMFLOAT2(0.4f, 0.4f));

	pApp->pSwapChain->Present(0, 0);
}

void SceneNormalMapping::Release()
{
	this->m_normalMapShader->Release();
	delete this->m_normalMapShader;
	this->m_normalMapShader = nullptr;
	this->m_modelLoader->Release();
	delete this->m_modelLoader;
	this->m_modelLoader = nullptr;
	this->m_lightBuffer->Release();
	this->m_lightBuffer = nullptr;
}
