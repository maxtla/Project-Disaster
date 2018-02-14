#include "ShadowMapShader.h"



bool ShadowMapShader::initializeShader(ID3D11Device * pDev, HWND hwnd, WCHAR * vtx_path, WCHAR * px_path)
{
	//collect local members here
	HRESULT hr;
	ID3D10Blob* errorMsg = NULL;
	ID3D10Blob* vertexShaderBuffer = NULL;
	ID3D10Blob* pixelShaderBuffer = NULL;
	D3D11_INPUT_ELEMENT_DESC layoutDescriptions[5];
	unsigned int numOfElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;

	//compile vertex shader from code file
	hr = D3DCompileFromFile(vtx_path,
		nullptr,
		nullptr,
		"shadowmap_vs_main",
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
		"shadowmap_ps_main",
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
	matrixBufferDesc.ByteWidth = sizeof(Matrices);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	hr = pDev->CreateBuffer(&matrixBufferDesc, NULL, &this->m_MatrixBuffer);
	if (FAILED(hr))
		return false;


	// Create a wrap texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	hr = pDev->CreateSamplerState(&samplerDesc, &this->m_WrapSampler);
	if (FAILED(hr))
	{
		return false;
	}

	// Create a comparison texture sampler state description.
	D3D11_SAMPLER_DESC comparisonSamplerDesc;
	ZeroMemory(&comparisonSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	comparisonSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.BorderColor[0] = 1.0f;
	comparisonSamplerDesc.BorderColor[1] = 1.0f;
	comparisonSamplerDesc.BorderColor[2] = 1.0f;
	comparisonSamplerDesc.BorderColor[3] = 1.0f;
	comparisonSamplerDesc.MinLOD = 0.f;
	comparisonSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	comparisonSamplerDesc.MipLODBias = 0.f;
	comparisonSamplerDesc.MaxAnisotropy = 0;
	comparisonSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	comparisonSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;

	// Create the texture sampler state.
	hr = pDev->CreateSamplerState(&comparisonSamplerDesc, &this->m_ComparisonSampler);
	if (FAILED(hr))
	{
		return false;
	}
	//now we are finally done here
	return true;
}

void ShadowMapShader::outputErrorMessage(ID3D10Blob * error, HWND hwnd, WCHAR * file)
{
	char* compileError;
	unsigned int bufferSize;
	ofstream fOut;

	//pointer to the error message text buffer
	compileError = (char*)(error->GetBufferPointer());
	//get message size
	bufferSize = (unsigned int)error->GetBufferSize();
	//open an error text file and write to it
	fOut.open("shadowMapShader_error.txt");
	for (unsigned int i = 0; i < bufferSize; i++)
	{
		fOut << compileError[i];
	}

	fOut.close();

	error->Release();
	error = NULL;

	MessageBox(hwnd, L"Error compiling shader.  Check shadowMapShader_error.txt", file, MB_OK);
	return;
}

bool ShadowMapShader::setShaderParameters(ID3D11DeviceContext * pDevCon, XMMATRIX world, XMMATRIX view, XMMATRIX projection, XMMATRIX lightView, XMMATRIX lightProjection, ID3D11ShaderResourceView * modelTexture, ID3D11ShaderResourceView * shadowMap)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	unsigned int bufferNr;
	Matrices* dataPtr;

	//transpose matrices since we have not specified anything in the compiler options
	world = XMMatrixTranspose(world);
	view = XMMatrixTranspose(view);
	projection = XMMatrixTranspose(projection);
	lightView = XMMatrixTranspose(lightView);
	lightProjection = XMMatrixTranspose(lightProjection);

	//lock the constant buffer for writing
	hr = pDevCon->Map(this->m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(hr))
		return false;

	//get the data pointer of the constant buffer
	dataPtr = (Matrices*)mappedSubresource.pData;
	//copy matrices
	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = projection;
	dataPtr->lightView = lightView;
	dataPtr->lightProjection = lightProjection;
	//unlock the constant buffer
	pDevCon->Unmap(this->m_MatrixBuffer, 0);
	//set the position of the constant buffer 
	bufferNr = 0;
	//finally set the constant buffer in vertex shader
	pDevCon->VSSetConstantBuffers(bufferNr, 1, &this->m_MatrixBuffer);

	pDevCon->PSSetShaderResources(0, 1, &modelTexture);
	pDevCon->PSSetShaderResources(1, 1, &shadowMap);

	return true;
}

void ShadowMapShader::RenderShader(ID3D11DeviceContext * pDevCon, int vertexCount)
{
	//set vertex input layout
	pDevCon->IASetInputLayout(this->m_InputLayout);
	//set the vertex and pixel shader
	pDevCon->VSSetShader(this->m_VertexShader, NULL, 0);
	pDevCon->PSSetShader(this->m_PixelShader, NULL, 0);
	//set the sampler states
	pDevCon->PSSetSamplers(0, 1, &this->m_WrapSampler);
	pDevCon->PSSetSamplers(1, 1, &this->m_ComparisonSampler);
	//render the geometry
	pDevCon->Draw(vertexCount, 0);

	return;
}

ShadowMapShader::ShadowMapShader()
{
	this->m_InputLayout = nullptr;
	this->m_MatrixBuffer = nullptr;
	this->m_PixelShader = nullptr;
	this->m_VertexShader = nullptr;
	this->m_ComparisonSampler = nullptr;
	this->m_WrapSampler = nullptr;
}


ShadowMapShader::~ShadowMapShader()
{
}

bool ShadowMapShader::Initialize(ID3D11Device * pDev, HWND hwnd)
{
	if (!this->initializeShader(pDev, hwnd, L"Shaders//shadowMapVS.hlsl", L"Shaders//shadowMapPS.hlsl"))
		return false;

	return true;
}

void ShadowMapShader::Release()
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
	if (this->m_ComparisonSampler)
	{
		this->m_ComparisonSampler->Release();
		this->m_ComparisonSampler = nullptr;
	}
	if (this->m_WrapSampler)
	{
		this->m_WrapSampler->Release();
		this->m_WrapSampler = nullptr;
	}
}

bool ShadowMapShader::Render(ID3D11DeviceContext * pDevCon, int vertexCount, XMMATRIX world, XMMATRIX view, XMMATRIX projection, XMMATRIX lightView, XMMATRIX lightProjection, ID3D11ShaderResourceView * modelTexture, ID3D11ShaderResourceView * shadowMap)
{
	if (!this->setShaderParameters(pDevCon, world, view, projection, lightView, lightProjection, modelTexture, shadowMap))
		return false;

	this->RenderShader(pDevCon, vertexCount);

	return true;
}
