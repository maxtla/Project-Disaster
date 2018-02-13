#include "NormalMapShader.h"



bool NormalMapShader::initializeShader(ID3D11Device * pDev, HWND hwnd, WCHAR * vertexFile, WCHAR * fragmentFile)
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
	hr = D3DCompileFromFile(vertexFile,
		nullptr,
		nullptr,
		"normalmap_vs_main",
		"vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		&vertexShaderBuffer,
		&errorMsg);

	if (FAILED(hr))
	{
		if (errorMsg)
		{
			this->outputErrorMessage(errorMsg, hwnd, vertexFile);
		}
		return false;
	}

	//compile fragemnt shader from code file
	hr = D3DCompileFromFile(fragmentFile,
		nullptr,
		nullptr,
		"normalmap_ps_main",
		"ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		&pixelShaderBuffer,
		&errorMsg);

	if (FAILED(hr))
	{
		if (errorMsg)
		{
			this->outputErrorMessage(errorMsg, hwnd, fragmentFile);
		}
		return false;
	}

	//create vertex and fragment shader from the buffers
	hr = pDev->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &this->_pVertexShader);
	if (FAILED(hr))
		return false;

	hr = pDev->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &this->_pPixelShader);
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
	hr = pDev->CreateInputLayout(layoutDescriptions, numOfElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &this->_pInputLayout);
	if (FAILED(hr))
		return false;

	//clean up a little
	vertexShaderBuffer->Release();
	vertexShaderBuffer = NULL;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = NULL;

	//prepare the sampler state description and create the sampler state
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

	hr = pDev->CreateSamplerState(&samplerDesc, &this->_pSamplerState);
	if (FAILED(hr))
		return false;

	//prepare the description of the matrix buffer and create the matrix buffer
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	hr = pDev->CreateBuffer(&matrixBufferDesc, NULL, &this->_pMatrixBuffer);
	if (FAILED(hr))
		return false;

	//now we are finally done here
	return true;
}

void NormalMapShader::ReleaseShader()
{
	if (this->_pInputLayout)
	{
		this->_pInputLayout->Release();
		this->_pInputLayout = NULL;
	}
	if (this->_pMatrixBuffer)
	{
		this->_pMatrixBuffer->Release();
		this->_pMatrixBuffer = NULL;
	}
	if (this->_pPixelShader)
	{
		this->_pPixelShader->Release();
		this->_pPixelShader = NULL;
	}
	if (this->_pSamplerState)
	{
		this->_pSamplerState->Release();
		this->_pSamplerState = NULL;
	}
	if (this->_pVertexShader)
	{
		this->_pVertexShader->Release();
		this->_pVertexShader = NULL;
	}
}

void NormalMapShader::outputErrorMessage(ID3D10Blob * blob, HWND hwnd, WCHAR * file)
{
	char* compileError;
	unsigned int bufferSize;
	ofstream fOut;

	//pointer to the error message text buffer
	compileError = (char*)(blob->GetBufferPointer());
	//get message size
	bufferSize = (unsigned int)blob->GetBufferSize();
	//open an error text file and write to it
	fOut.open("normalMapShader_error.txt");
	for (unsigned int i = 0; i < bufferSize; i++)
	{
		fOut << compileError[i];
	}

	fOut.close();

	blob->Release();
	blob = NULL;

	MessageBox(hwnd, L"Error compiling shader.  Check normalMapShader_error.txt", file, MB_OK);
	return;
}

bool NormalMapShader::setShaderParameters(ID3D11DeviceContext * pDevCon, XMMATRIX world, XMMATRIX view, XMMATRIX projection, ID3D11ShaderResourceView * pTexture, ID3D11ShaderResourceView * pNormalMap, bool renderNormalMap, ID3D11Buffer * materialBuffer, ID3D11Buffer* lightBuffer)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	unsigned int bufferNr;
	MatrixBufferType* dataPtr;

	//transpose matrices since we have not specified anything in the compiler options
	world = XMMatrixTranspose(world);
	view = XMMatrixTranspose(view);
	projection = XMMatrixTranspose(projection);

	//lock the constant buffer for writing
	hr = pDevCon->Map(this->_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(hr))
		return false;

	//get the data pointer of the constant buffer
	dataPtr = (MatrixBufferType*)mappedSubresource.pData;
	//copy matrices
	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = projection;
	//unlock the constant buffer
	pDevCon->Unmap(this->_pMatrixBuffer, 0);
	//set the position of the constant buffer 
	bufferNr = 0;
	//finally set the constant buffer in vertex shader
	pDevCon->VSSetConstantBuffers(bufferNr, 1, &this->_pMatrixBuffer);
	//also set the shader texture for the pixel shader
	pDevCon->PSSetShaderResources(0, 1, &pTexture);
	pDevCon->PSSetShaderResources(1, 1, &pNormalMap);
	//prepare and set the pixel shade constant buffers
	pDevCon->PSSetConstantBuffers(0, 1, &lightBuffer);
	pDevCon->PSSetConstantBuffers(1, 1, &materialBuffer);

	return true;
}

void NormalMapShader::renderShader(ID3D11DeviceContext * pDevCon, int vertexCount, ID3D11SamplerState * texSampler)
{
	//set vertex input layout
	pDevCon->IASetInputLayout(this->_pInputLayout);
	//set the vertex and pixel shader
	pDevCon->VSSetShader(this->_pVertexShader, NULL, 0);
	pDevCon->PSSetShader(this->_pPixelShader, NULL, 0);
	//set sampler states 
	pDevCon->PSSetSamplers(0, 1, &texSampler);
	//render the geometry
	pDevCon->Draw(vertexCount, 0);

	return;
}

NormalMapShader::NormalMapShader()
{
	this->_pInputLayout = NULL;
	this->_pMatrixBuffer = NULL;
	this->_pPixelShader = NULL;
	this->_pSamplerState = NULL;
	this->_pVertexShader = NULL;
}


NormalMapShader::~NormalMapShader()
{
}

bool NormalMapShader::initialize(ID3D11Device * pDev, HWND hwnd)
{
	bool result;
	//load in the vertex and pixel shader files
	result = this->initializeShader(pDev, hwnd, L"Shaders//normalMapVS.hlsl", L"Shaders//normalMapPS.hlsl");
	if (!result)
		return false;

	return true;
}

void NormalMapShader::Release()
{
	this->ReleaseShader();
	return;
}

bool NormalMapShader::render(ID3D11DeviceContext * pDevCon, int index, XMMATRIX world, XMMATRIX view, XMMATRIX projection, ID3D11ShaderResourceView * pTexture, ID3D11ShaderResourceView * pNormalMap, bool renderNormalMap, ID3D11SamplerState * texSampler, ID3D11Buffer* materialBuffer, ID3D11Buffer * lightBuffer)
{
	bool result;
	//set parameters used for rendering
	result = this->setShaderParameters(pDevCon, world, view, projection, pTexture, pNormalMap, renderNormalMap, materialBuffer, lightBuffer);
	if (!result)
		return false;

	//render the prepared buffers
	this->renderShader(pDevCon, index, texSampler);
	return true;
}
