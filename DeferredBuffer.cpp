#include "DeferredBuffer.h"



DeferredBuffer::DeferredBuffer()
{
	for (int i = 0; i < ARR_SIZE; i++)
	{
		this->_RTTArr[i] = NULL;
		this->_RTVArr[i] = NULL;
		this->_SRVArr[i] = NULL;
	}
	this->_DSBuffer = NULL;
	this->_DSV = NULL;
}


DeferredBuffer::~DeferredBuffer()
{
}

bool DeferredBuffer::initialize(ID3D11Device * pDev, int texWidth, int texHeight, float farPlane, float nearPlane)
{
	//collect variables here
	HRESULT hr;
	D3D11_TEXTURE2D_DESC rttDesc;
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	D3D11_TEXTURE2D_DESC dbDesc; //depth buffer desc.
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;

	//save the width and height of the texture
	this->_texWidth = texWidth;
	this->_texHeight = texHeight;

	//prepare RTT description
	ZeroMemory(&rttDesc, sizeof(D3D11_TEXTURE2D_DESC));
	rttDesc.Width = texWidth;
	rttDesc.Height = texHeight;
	rttDesc.MipLevels = 1;
	rttDesc.ArraySize = 1;
	rttDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //this is good for precision
	rttDesc.SampleDesc.Count = 1;
	rttDesc.Usage = D3D11_USAGE_DEFAULT;
	rttDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	rttDesc.CPUAccessFlags = 0;
	rttDesc.MiscFlags = 0;

	//create render target textures
	for (int i = 0; i < ARR_SIZE; i++)
	{
		hr = pDev->CreateTexture2D(&rttDesc, NULL, &this->_RTTArr[i]);
		if (FAILED(hr))
			return false;
	}

	//prepare RTV description
	rtvDesc.Format = rttDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	//create render target views
	for (int i = 0; i < ARR_SIZE; i++)
	{
		hr = pDev->CreateRenderTargetView(this->_RTTArr[i], &rtvDesc, &this->_RTVArr[i]);
		if (FAILED(hr))
			return false;
	}

	//prepare shader resource view description
	srvDesc.Format = rttDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	//create shader resource views
	for (int i = 0; i < ARR_SIZE; i++)
	{
		hr = pDev->CreateShaderResourceView(this->_RTTArr[i], &srvDesc, &this->_SRVArr[i]);
		if (FAILED(hr))
			return false;
	}

	//prepare depth buffer description
	ZeroMemory(&dbDesc, sizeof(D3D11_TEXTURE2D_DESC));
	dbDesc.Width = texWidth;
	dbDesc.Height = texHeight;
	dbDesc.MipLevels = 1;
	dbDesc.ArraySize = 1;
	dbDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dbDesc.SampleDesc.Count = 1;
	dbDesc.SampleDesc.Quality = 0;
	dbDesc.Usage = D3D11_USAGE_DEFAULT;
	dbDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dbDesc.CPUAccessFlags = 0;
	dbDesc.MiscFlags = 0;

	//create the texture for the depth buffer
	hr = pDev->CreateTexture2D(&dbDesc, NULL, &this->_DSBuffer);
	if (FAILED(hr))
		return false;

	//prepare depth stencil view description
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	//create depth stencil view
	hr = pDev->CreateDepthStencilView(this->_DSBuffer, &dsvDesc, &this->_DSV);
	if (FAILED(hr))
		return false;

	//prepare viewport
	this->_viewport.Width = (float)texWidth;
	this->_viewport.Height = (float)texHeight;
	this->_viewport.MinDepth = 0.0f;
	this->_viewport.MaxDepth = 1.0f;
	this->_viewport.TopLeftX = 0.0f;
	this->_viewport.TopLeftY = 0.0f;

	return true;
}

void DeferredBuffer::Release()
{
	if (this->_DSV)
	{
		this->_DSV->Release();
		this->_DSV = NULL;
	}
	if (this->_DSBuffer)
	{
		this->_DSBuffer->Release();
		this->_DSBuffer = NULL;
	}
	for (int i = 0; i < ARR_SIZE; i++)
	{
		if (this->_SRVArr[i])
		{
			this->_SRVArr[i]->Release();
			this->_SRVArr[i] = NULL;
		}
		if (this->_RTVArr[i])
		{
			this->_RTVArr[i]->Release();
			this->_RTVArr[i] = NULL;
		}
		if (this->_RTTArr[i])
		{
			this->_RTTArr[i]->Release();
			this->_RTTArr[i] = NULL;
		}
	}
	return;
}

void DeferredBuffer::setRenderTargets(ID3D11DeviceContext * pDevCon)
{
	//bind the RTV array and DSBuffer to the output render pipeline
	pDevCon->OMSetRenderTargets(ARR_SIZE, this->_RTVArr, this->_DSV);
	//set view port
	pDevCon->RSSetViewports(1, &this->_viewport);
	return;
}

void DeferredBuffer::clearRenderTargets(ID3D11DeviceContext * pDevCon, float r, float g, float b, float a)
{
	//color to clear the buffer with
	float color[4];
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;

	//clear RTV buffers
	for (int i = 0; i < ARR_SIZE; i++)
	{
		pDevCon->ClearRenderTargetView(this->_RTVArr[i], color);
	}
	//clear depth buffer
	pDevCon->ClearDepthStencilView(this->_DSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	return;
}

ID3D11ShaderResourceView * DeferredBuffer::getShaderResourceView(int view)
{
	return this->_SRVArr[view];
}

D3D11_VIEWPORT* DeferredBuffer::getViewPort()
{
	return &this->_viewport;
}
