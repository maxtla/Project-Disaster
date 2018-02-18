#include "Application.h"



Application::Application()
{
	this->pSwapChain = nullptr;
	this->pDev = nullptr;
	this->pDevCon = nullptr;
	this->pRTV = nullptr;
	this->pDSV = nullptr;
	this->pDSBuffer = nullptr;
	this->pDepthStencilState = nullptr;
	this->pDepthDisabledStencilState = nullptr;
	this->pRasterState = nullptr;

	this->inputHandler = nullptr;

	this->pSceneDefRender = nullptr;
	this->pSceneNormalMap = nullptr;
	this->pSceneShadowMap = nullptr;
	this->pSceneHeightMap = nullptr;
}


Application::~Application()
{
}

bool Application::initApplication(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT hr;
	//buffer description
	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = WIDTH;
	bufferDesc.Height = HEIGHT;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//swapchain description
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 4;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//create swapchain
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDev, NULL, &pDevCon);
	if (FAILED(hr))
	{
		exit(-1);
	}
	//create backbuffer
	ID3D11Texture2D* backBuffer;
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (FAILED(hr))
	{
		exit(-1);
	}
	//create RTV
	hr = pDev->CreateRenderTargetView(backBuffer, NULL, &pRTV);
	backBuffer->Release();
	if (FAILED(hr))
	{
		exit(-1);
	}

	//describe the DSBuffer (depth-stencil buffer)
	D3D11_TEXTURE2D_DESC dpDesc;
	ZeroMemory(&dpDesc, sizeof(D3D11_TEXTURE2D_DESC));

	dpDesc.Width = WIDTH;
	dpDesc.Height = HEIGHT;
	dpDesc.MipLevels = 1;
	dpDesc.ArraySize = 1;
	dpDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dpDesc.SampleDesc.Count = 4;
	dpDesc.SampleDesc.Quality = 0;
	dpDesc.Usage = D3D11_USAGE_DEFAULT;
	dpDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dpDesc.CPUAccessFlags = 0;
	dpDesc.MiscFlags = 0;

	//create depth stecil view
	pDev->CreateTexture2D(&dpDesc, NULL, &pDSBuffer);


	//set up depth stencil desc
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//create the normal depth stencil state
	hr = pDev->CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState);
	if (FAILED(hr))
		return false;
	//Set the depth stencil state
	pDevCon->OMSetDepthStencilState(pDepthStencilState, 1);
	//initialize depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	//prepare the depth stencil view desc
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	//create depth stencil view
	pDev->CreateDepthStencilView(pDSBuffer, &depthStencilViewDesc, &pDSV);
	//set render target and bind depth-stencil view object
	pDevCon->OMSetRenderTargets(1, &pRTV, pDSV);
	//create and set the raster description and raster state
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = true;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.MultisampleEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	//create rasterizer state
	hr = pDev->CreateRasterizerState(&rasterDesc, &pRasterState);
	if (FAILED(hr))
		return false;
	//set rasterizer state
	pDevCon->RSSetState(pRasterState);

	// Setup the viewport for rendering.
	_viewPort.Width = (float)WIDTH;
	_viewPort.Height = (float)HEIGHT;
	_viewPort.MinDepth = 0.0f;
	_viewPort.MaxDepth = 1.0f;
	_viewPort.TopLeftX = 0.0f;
	_viewPort.TopLeftY = 0.0f;

	// Create the viewport.
	pDevCon->RSSetViewports(1, &_viewPort);

	//create the second depth stencil state for 2D rendering (Z buffer disabled)
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = pDev->CreateDepthStencilState(&depthDisabledStencilDesc, &pDepthDisabledStencilState);
	if (FAILED(hr))
		return false;

	this->view = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -3.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
	this->projection = XMMatrixPerspectiveFovLH(XM_PI * 0.45f, ((float)WIDTH) / HEIGHT, 0.01f, 500.0f);

	this->inputHandler = new Movement();
	this->inputHandler->initialize(hwnd);

	m_font = std::make_unique<SpriteFont>(pDev, L"myfile.spritefont");
	m_spriteBatch = std::make_unique<SpriteBatch>(pDevCon);
	m_fontPos = XMFLOAT2(350.f, 50.f);

	return true;
}

bool Application::initScenes(HINSTANCE hInstance, HWND hwnd)
{
	if (pSceneDefRender == nullptr)
		pSceneDefRender = new SceneDeferredRendering();

	if (!this->pSceneDefRender->initScene(this, hInstance, hwnd))
		return false;

	if (pSceneNormalMap == nullptr)
		pSceneNormalMap = new SceneNormalMapping();

	if (!this->pSceneNormalMap->initScene(this, hInstance, hwnd))
		return false;

	if (pSceneShadowMap == nullptr)
		pSceneShadowMap = new SceneShadowMapping();

	if (!this->pSceneShadowMap->initScene(this, hInstance, hwnd))
		return false;

	if (pSceneHeightMap == nullptr)
		pSceneHeightMap = new SceneHeightMap();

	if (!this->pSceneHeightMap->initScene(this, hInstance, hwnd))
		return false;

	this->currentScene = Scenes::SceneOne;

	return true;
}

void Application::handleInput()
{

}

void Application::update()
{
	
	this->inputHandler->detectKeys(this->currentScene);
	

	switch (this->currentScene)
	{
	case Scenes::SceneOne:
		this->pSceneDefRender->updateScene();
		break;
	case Scenes::SceneTwo:
		this->pSceneNormalMap->updateScene();
		break;
	case Scenes::SceneThree:
		this->pSceneShadowMap->updateScene();
		break;
	case Scenes::SceneFour:
		this->pSceneHeightMap->updateScene();
		break;
	default:
		break;
	}
	
}

void Application::render()
{
	this->inputHandler->updateCamera(view);
	switch (this->currentScene)
	{
	case Scenes::SceneOne:
		this->pSceneDefRender->renderScene(this);
		break;
	case Scenes::SceneTwo:
		this->pSceneNormalMap->renderScene(this);
		break;
	case Scenes::SceneThree:
		this->pSceneShadowMap->renderScene(this);
		break;
	case Scenes::SceneFour:
		this->pSceneHeightMap->renderScene(this);
		break;
	default:
		break;
	}
}

void Application::Release()
{
	if (this->pSwapChain)
		this->pSwapChain->Release();
	if (this->pDev)
		this->pDev->Release();
	if (this->pDevCon)
		this->pDevCon->Release();
	if (this->pRTV)
		this->pRTV->Release();
	if (this->pDSV)
		this->pDSV->Release();
	if (this->pDSBuffer)
		this->pDSBuffer->Release();
	if (this->pDepthStencilState)
		this->pDepthStencilState->Release();
	if (this->pDepthDisabledStencilState)
		this->pDepthDisabledStencilState->Release();
	if (this->pRasterState)
		this->pRasterState->Release();

	if (this->pSceneDefRender)
		this->pSceneDefRender->Release();
	if (this->pSceneNormalMap)
		this->pSceneNormalMap->Release();
	if (this->pSceneShadowMap)
		this->pSceneShadowMap->Release();
}

void Application::switchScene()
{
	this->currentScene = (this->currentScene + 1) % NUMOFSCENES;
	if (this->currentScene == 2)
	{
		this->view = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.3f, -2.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
	}
	else
	{
		this->view = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -3.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
	}
}

void Application::textToScreen(wstring text, XMFLOAT2 position, XMFLOAT2 scaling)
{
	m_spriteBatch->Begin();
	m_font->DrawString(m_spriteBatch.get(), text.c_str(), XMLoadFloat2(&position), Colors::White, 0.f, g_XMZero, XMLoadFloat2(&scaling));
	m_spriteBatch->End();
}

void Application::camInfoToScreen(XMFLOAT2 position, XMFLOAT2 scaling)
{
	//prepare the wstrings for output
	XMFLOAT3 pos, target;
	XMStoreFloat3(&pos, this->inputHandler->getCamPos());
	XMStoreFloat3(&target, this->inputHandler->getTarget());

	wstring ws_pos = L"Camera position: X: " + to_wstring(pos.x) + L" Y: " + to_wstring(pos.y) + L" Z: " + to_wstring(pos.z);
	wstring ws_target = L"Target position: X: " + to_wstring(target.x) + L" Y: " + to_wstring(target.y) + L" Z: " + to_wstring(target.z);

	textToScreen(ws_pos, position, scaling);
	textToScreen(ws_target, XMFLOAT2(position.x, position.y + 35.f), scaling);
}
