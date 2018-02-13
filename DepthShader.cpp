#include "DepthShader.h"



bool DepthShader::initializeShader(ID3D11Device * pDev, HWND hwnd, WCHAR * vtx_path, WCHAR * px_path)
{
	//collect local members here
	HRESULT hr;
	ID3D10Blob* errorMsg = NULL;
	ID3D10Blob* vertexShaderBuffer = NULL;
	ID3D10Blob* pixelShaderBuffer = NULL;
	D3D11_INPUT_ELEMENT_DESC layoutDescriptions[5];
	unsigned int numOfElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	//compile vertex shader from code file
	hr = D3DCompileFromFile(vtx_path,
		nullptr,
		nullptr,
		"depthshader_vs_main",
		"vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		&vertexShaderBuffer,
		&errorMsg);

	if (FAILED(hr))
	{
		if (errorMsg)
		{
			this->outputErrorMessage(errorMsg, hwnd, vtx_path);
		}
		return false;
	}

	//compile fragemnt shader from code file
	hr = D3DCompileFromFile(px_path,
		nullptr,
		nullptr,
		"depthshader_ps_main",
		"ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		&pixelShaderBuffer,
		&errorMsg);

	if (FAILED(hr))
	{
		if (errorMsg)
		{
			this->outputErrorMessage(errorMsg, hwnd, px_path);
		}
		return false;
	}

	//create vertex and fragment shader from the buffers
	hr = pDev->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &this->m_VertexShader);
	if (FAILED(hr))
		return false;

	hr = pDev->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &this->m_PixelShader);
	if (FAILED(hr))
		return false;

	//create the input layout for the vertex shader
	layoutDescriptions[0].SemanticName = "POSITION";
	layoutDescriptions[0].SemanticIndex = 0;
	layoutDescriptions[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDescriptions[0].InputSlot = 0;
	layoutDescriptions[0].AlignedByteOffset = 0;
	layoutDescriptions[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDescriptions[0].InstanceDataStepRate = 0;

	layoutDescriptions[1].SemanticName = "TEXCOORD";
	layoutDescriptions[1].SemanticIndex = 0;
	layoutDescriptions[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	layoutDescriptions[1].InputSlot = 0;
	layoutDescriptions[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDescriptions[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDescriptions[1].InstanceDataStepRate = 0;

	layoutDescriptions[2].SemanticName = "NORMAL";
	layoutDescriptions[2].SemanticIndex = 0;
	layoutDescriptions[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDescriptions[2].InputSlot = 0;
	layoutDescriptions[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDescriptions[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDescriptions[2].InstanceDataStepRate = 0;

	layoutDescriptions[3].SemanticName = "TANGENT";
	layoutDescriptions[3].SemanticIndex = 0;
	layoutDescriptions[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDescriptions[3].InputSlot = 0;
	layoutDescriptions[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDescriptions[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDescriptions[3].InstanceDataStepRate = 0;

	layoutDescriptions[4].SemanticName = "BINORMAL";
	layoutDescriptions[4].SemanticIndex = 0;
	layoutDescriptions[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDescriptions[4].InputSlot = 0;
	layoutDescriptions[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDescriptions[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDescriptions[4].InstanceDataStepRate = 0;

	//get a count of the elements
	numOfElements = sizeof(layoutDescriptions) / sizeof(layoutDescriptions[0]);

	//create the vertex input layout
	hr = pDev->CreateInputLayout(layoutDescriptions, numOfElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &this->m_InputLayout);
	if (FAILED(hr))
		return false;

	//clean up a little
	vertexShaderBuffer->Release();
	vertexShaderBuffer = NULL;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = NULL;

	//prepare the description of the matrix buffer and create the matrix buffer
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferStruct);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	hr = pDev->CreateBuffer(&matrixBufferDesc, NULL, &this->m_MatrixBuffer);
	if (FAILED(hr))
		return false;

	if (!this->initializeDepthBuffer(pDev))
		return false;
	//now we are finally done here
	return true;
}

bool DepthShader::initializeDepthBuffer(ID3D11Device * pDev)
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC rttDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	//prepare RTT description
	ZeroMemory(&rttDesc, sizeof(D3D11_TEXTURE2D_DESC));
	rttDesc.Width = SHADOWMAP_WIDTH;
	rttDesc.Height = SHADOWMAP_HEIGHT;
	rttDesc.MipLevels = 1;
	rttDesc.ArraySize = 1;
	rttDesc.Format = DXGI_FORMAT_R24G8_TYPELESS; //this is good for precision
	rttDesc.SampleDesc.Count = 1;
	rttDesc.SampleDesc.Quality = 0;
	rttDesc.Usage = D3D11_USAGE_DEFAULT;
	rttDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	rttDesc.CPUAccessFlags = 0;
	rttDesc.MiscFlags = 0;

	hr = pDev->CreateTexture2D(&rttDesc, NULL, &this->m_depthBufferTexture);
	if (FAILED(hr))
		return false;

	//prepare DSV desc
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	hr = pDev->CreateDepthStencilView(this->m_depthBufferTexture, &dsvDesc, &this->m_DSV);
	if (FAILED(hr))
		return false;

	//prepare shader resource view description
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	//create shader resource views
	hr = pDev->CreateShaderResourceView(this->m_depthBufferTexture, &srvDesc, &this->m_SRV);
	if (FAILED(hr))
		return false;


	D3D11_RASTERIZER_DESC shadowRenderStateDesc;
	ZeroMemory(&shadowRenderStateDesc, sizeof(D3D11_RASTERIZER_DESC));
	shadowRenderStateDesc.CullMode = D3D11_CULL_FRONT;
	shadowRenderStateDesc.FillMode = D3D11_FILL_SOLID;
	shadowRenderStateDesc.DepthClipEnable = true;

	hr = pDev->CreateRasterizerState(&shadowRenderStateDesc, &this->m_RenderState);
	if (FAILED(hr))
		return false;

	//set the viewport
	this->m_Viewport.Height = (float)SHADOWMAP_HEIGHT;
	this->m_Viewport.Width = (float)SHADOWMAP_WIDTH;
	this->m_Viewport.MinDepth = 0.f;
	this->m_Viewport.MaxDepth = 1.f;

	return true;
}

void DepthShader::outputErrorMessage(ID3D10Blob * error, HWND hwnd, WCHAR * file)
{
	char* compileError;
	unsigned int bufferSize;
	ofstream fOut;

	//pointer to the error message text buffer
	compileError = (char*)(error->GetBufferPointer());
	//get message size
	bufferSize = (unsigned int)error->GetBufferSize();
	//open an error text file and write to it
	fOut.open("depthShader_error.txt");
	for (unsigned int i = 0; i < bufferSize; i++)
	{
		fOut << compileError[i];
	}

	fOut.close();

	error->Release();
	error = NULL;

	MessageBox(hwnd, L"Error compiling shader.  Check depthShader_error.txt", file, MB_OK);
	return;
}

bool DepthShader::setShaderParameters(ID3D11DeviceContext * pDevCon, XMMATRIX world, XMMATRIX view, XMMATRIX projection)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	unsigned int bufferNr;
	MatrixBufferStruct* dataPtr;

	//transpose matrices since we have not specified anything in the compiler options
	world = XMMatrixTranspose(world);
	view = XMMatrixTranspose(view);
	projection = XMMatrixTranspose(projection);

	//lock the constant buffer for writing
	hr = pDevCon->Map(this->m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(hr))
		return false;

	//get the data pointer of the constant buffer
	dataPtr = (MatrixBufferStruct*)mappedSubresource.pData;
	//copy matrices
	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = projection;
	//unlock the constant buffer
	pDevCon->Unmap(this->m_MatrixBuffer, 0);
	//set the position of the constant buffer 
	bufferNr = 0;
	//finally set the constant buffer in vertex shader
	pDevCon->VSSetConstantBuffers(bufferNr, 1, &this->m_MatrixBuffer);

	return true;
}

void DepthShader::RenderShader(ID3D11DeviceContext * pDevCon, int vertexCount)
{
	//set vertex input layout
	pDevCon->IASetInputLayout(this->m_InputLayout);
	//set the vertex and pixel shader
	pDevCon->VSSetShader(this->m_VertexShader, NULL, 0);
	pDevCon->PSSetShader(this->m_PixelShader, NULL, 0);
	//render the geometry
	pDevCon->Draw(vertexCount, 0);

	return;
}

DepthShader::DepthShader()
{
	this->m_InputLayout = nullptr;
	this->m_MatrixBuffer = nullptr;
	this->m_PixelShader = nullptr;
	this->m_VertexShader = nullptr;
	this->m_depthBufferTexture = nullptr;
	this->m_DSV = nullptr;
	this->m_SRV = nullptr;
	this->m_RenderState = nullptr;
}


DepthShader::~DepthShader()
{
}

bool DepthShader::Initialize(ID3D11Device * pDev, HWND hwnd)
{
	if (!this->initializeShader(pDev, hwnd, L"Shaders//depthShaderVS.hlsl", L"Shaders//depthShaderPS.hlsl"))
		return false;

	return true;
}

void DepthShader::Release()
{
	if (this->m_InputLayout)
	{
		this->m_InputLayout->Release();
		this->m_InputLayout = nullptr;
	}
	if (this->m_MatrixBuffer)
	{
		this->m_MatrixBuffer->Release();
		this->m_MatrixBuffer = nullptr;
	}
	if (this->m_PixelShader)
	{
		this->m_PixelShader->Release();
		this->m_PixelShader = nullptr;
	}
	if (this->m_VertexShader)
	{
		this->m_VertexShader->Release();
		this->m_VertexShader = nullptr;
	}
	if (this->m_depthBufferTexture)
	{
		this->m_depthBufferTexture->Release();
		this->m_depthBufferTexture = nullptr;
	}
	if (this->m_DSV)
	{
		this->m_DSV->Release();
		this->m_DSV = nullptr;
	}
	if (this->m_SRV)
	{
		this->m_SRV->Release();
		this->m_SRV = nullptr;
	}
	if (this->m_RenderState)
	{
		this->m_RenderState->Release();
		this->m_RenderState = nullptr;
	}
	
}

bool DepthShader::Render(ID3D11DeviceContext * pDevCon, int vertexCount, XMMATRIX world, XMMATRIX view, XMMATRIX projection)
{
	if (!this->setShaderParameters(pDevCon, world, view, projection))
		return false;

	this->RenderShader(pDevCon, vertexCount);
	return true;
}

bool DepthShader::setRenderTarget(ID3D11DeviceContext * pDevCon)
{
	//bind the depth buffer RTV to the output render pipeline
	pDevCon->OMSetRenderTargets(0, nullptr, this->m_DSV);
	pDevCon->RSSetState(this->m_RenderState);
	pDevCon->RSSetViewports(1, &this->m_Viewport);
	//clear the DSV
	pDevCon->ClearDepthStencilView(this->m_DSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	return true;
}
