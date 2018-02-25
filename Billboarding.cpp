#include "Billboarding.h"


void Billboarding::outputErrorMessage(ID3D10Blob * blob, HWND hwnd, WCHAR * file)
{
	char* compileError;
	unsigned int bufferSize;
	ofstream fOut;

	//pointer to the error message text buffer
	compileError = (char*)(blob->GetBufferPointer());
	//get message size
	bufferSize = (unsigned int)blob->GetBufferSize();
	//open an error text file and write to it
	fOut.open("shader_error.txt");
	for (unsigned int i = 0; i < bufferSize; i++)
	{
		fOut << compileError[i];
	}

	fOut.close();

	blob->Release();
	blob = NULL;

	MessageBox(hwnd, L"Error compiling shader.  Check shader_error.txt", file, MB_OK);
	return;
}

bool Billboarding::setShaderParameters(ID3D11DeviceContext * pDevCon, XMMATRIX world, XMMATRIX view, XMMATRIX proj, ID3D11ShaderResourceView * pTexture, XMVECTOR camPos)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	unsigned int bufferNr;
	MatrixBufferStruct* dataPtr;
	GeometryBufferStruct geoDataPtr;

	world = XMMatrixTranspose(world);
	view = XMMatrixTranspose(view);
	proj = XMMatrixTranspose(proj);

	//lock Constant buffer for writing
	hr = pDevCon->Map(this->_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(hr))
		return false;

	//get data point of the constant buffer
	dataPtr = (MatrixBufferStruct*)mappedSubresource.pData;
	dataPtr->worldPos = world;
	dataPtr->view = view;
	dataPtr->projection = proj;

	pDevCon->Unmap(this->_pMatrixBuffer, 0);

	bufferNr = 0;

	pDevCon->VSSetConstantBuffers(bufferNr, 1, &this->_pMatrixBuffer);
	pDevCon->GSSetConstantBuffers(0, 1, &this->_pGeometryBuffer);

	geoDataPtr.camPos = camPos;
	pDevCon->UpdateSubresource(this->_pGeometryBuffer, 0, nullptr, &geoDataPtr, 0, 0);

	pDevCon->PSSetShaderResources(0, 1, &pTexture);
	return true;
}

void Billboarding::renderShader(ID3D11DeviceContext * pDevCon, int iCount, ID3D11SamplerState * texSampler)
{
	pDevCon->IASetInputLayout(this->_pInputLayout);

	pDevCon->VSSetShader(this->_pVertexShader, NULL, 0);
	pDevCon->GSSetShader(this->_pGeometryShader, NULL, 0);
	pDevCon->PSSetShader(this->_pPixelShader, NULL, 0);

	pDevCon->PSSetSamplers(0, 1, &texSampler);
	pDevCon->Draw(iCount, 0);
}

void Billboarding::releaseShader()
{
	this->Release();
	return;
}

Billboarding::Billboarding()
{
	this->_pInputLayout = NULL;
	this->_pMatrixBuffer = NULL;
	this->_pPixelShader = NULL;
	this->_pSamplerState = NULL;
	this->_pVertexShader = NULL;
	this->_pGeometryShader = NULL;
}

Billboarding::~Billboarding()
{
}

bool Billboarding::initializeShader(ID3D11Device * pDev, HWND hwnd, WCHAR* geometryFile, WCHAR* fragmentFile, WCHAR* vertexFile)
{
	HRESULT hr;
	ID3D10Blob* errorMsg = NULL;
	ID3D10Blob* geometryShaderBuffer = NULL;
	ID3D10Blob* vertexShaderBuffer = NULL;
	ID3D10Blob* pixelShaderBuffer = NULL;
	D3D11_INPUT_ELEMENT_DESC layoutDescriptions[3];
	unsigned int numOfElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc, geometryBufferDesc;

	//TO DO måste skriva en PS och GS......
	hr = D3DCompileFromFile(geometryFile, 
		nullptr, nullptr, 
		"bill_gs_main", 
		"gs_5_0", 
		D3D10_SHADER_ENABLE_STRICTNESS,
		0, 
		&geometryShaderBuffer, 
		&errorMsg);

	if (FAILED(hr))
	{
		if (errorMsg)
		{
			this->outputErrorMessage(errorMsg, hwnd, geometryFile);
		}
		return false;
	}

	hr = D3DCompileFromFile(fragmentFile,
		nullptr, nullptr,
		"bill_ps_main",
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

	hr = D3DCompileFromFile(vertexFile,
		nullptr, nullptr,
		"bill_vs_main",
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

	hr = pDev->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &this->_pVertexShader);
	if (FAILED(hr))
		return false;

	hr = pDev->CreateGeometryShader(geometryShaderBuffer->GetBufferPointer(), geometryShaderBuffer->GetBufferSize(), NULL, &this->_pGeometryShader);
	if (FAILED(hr))
		return false;

	hr = pDev->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &this->_pPixelShader);
	//TO DO... Fortsätta implementera allt detta

	hr = pDev->CreateBuffer(&geometryBufferDesc, NULL, &this->_pGeometryBuffer);
	if (FAILED(hr))
		return false;

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

	numOfElements = sizeof(layoutDescriptions) / sizeof(layoutDescriptions[0]);

	hr = pDev->CreateInputLayout(layoutDescriptions, numOfElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &this->_pInputLayout);
	if (FAILED(hr))
		return false;

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
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferStruct);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	hr = pDev->CreateBuffer(&matrixBufferDesc, NULL, &this->_pMatrixBuffer);
	if (FAILED(hr))
		return false;

	//prepare the description of the matrix buffer and create the matrix buffer
	geometryBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	geometryBufferDesc.ByteWidth = sizeof(GeometryBufferStruct);
	geometryBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	geometryBufferDesc.CPUAccessFlags = 0;
	geometryBufferDesc.MiscFlags = 0;
	geometryBufferDesc.StructureByteStride = 0;

	hr = pDev->CreateBuffer(&geometryBufferDesc, NULL, &this->_pGeometryBuffer);
	if (FAILED(hr))
		return false;

	return true;
}

bool Billboarding::initialize(ID3D11Device * pDev, HWND hwnd)
{
	bool result;

	result = this->initializeShader(pDev, hwnd, L"Shaders//BillGeometryShader", L"Shaders//BillPixelShader", L"Shaders//BillVertexShader");
	if (!result)
		return result;
	return result;
}

void Billboarding::Release()
{
	if (this->_pGeometryShader)
	{
		this->_pGeometryShader->Release();
		this->_pGeometryShader = NULL;
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

bool Billboarding::render(ID3D11DeviceContext * pDevCon, int index, XMMATRIX world, XMMATRIX view, XMMATRIX proj, ID3D11ShaderResourceView * pTexture, ID3D11SamplerState * texSampler, XMVECTOR camPos)
{
	bool result;

	result = this->setShaderParameters(pDevCon, world, view, proj, pTexture, camPos);

	this->renderShader(pDevCon, index, texSampler);
	return true;
}
