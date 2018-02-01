#include "LightShader.h"




LightShader::LightShader()
{
	this->_pLayout = nullptr;
	this->_pLightBuffer = nullptr;
	this->_pMatrixBuffer = nullptr;
	this->_pPixelShader = nullptr;
	this->_pSampleState = nullptr;
	this->_pVertexShader = nullptr;
	this->_pVertexBuffer = nullptr;
	this->initFullScreenQuad();
}


LightShader::~LightShader()
{
}

bool LightShader::initialize(ID3D11Device * pDev, HWND hwnd)
{
	if (!this->InitializeShader(pDev, hwnd, L"Shaders//LightVertexShader.hlsl", L"Shaders//LightPixelShader.hlsl"))
		return false;

	return true;
}

void LightShader::Release()
{
	this->ReleaseShader();
}

bool LightShader::Render(ID3D11DeviceContext * pDevCon, XMMATRIX world, XMMATRIX view, XMMATRIX projection, ID3D11ShaderResourceView * colorTexture, ID3D11ShaderResourceView * normalTexture, ID3D11ShaderResourceView* positionTexture, ID3D11Buffer* materialBuffer, XMVECTOR lightDir)
{
	if (!SetShaderParameters(pDevCon, world, view, projection, colorTexture, normalTexture, positionTexture, materialBuffer, lightDir))
		return false;

	RenderShader(pDevCon);

	return true;
}


void LightShader::initFullScreenQuad()
{
	this->_fullScreenQuad.vertices.resize(6);

	this->_fullScreenQuad.vertices[0] = { { -1.0f, -1.0f, 0.0f },{ 0.0f, 1.0f } };
	this->_fullScreenQuad.vertices[1] = { { -1.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } };
	this->_fullScreenQuad.vertices[2] = { { 1.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } };
	this->_fullScreenQuad.vertices[3] = { { -1.0f, -1.0f, 0.0f },{ 0.0f, 1.0f } };
	this->_fullScreenQuad.vertices[4] = { { 1.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } };
	this->_fullScreenQuad.vertices[5] = { { 1.0f, -1.0f, 0.0f },{ 1.0f, 1.0f } };
}

bool LightShader::InitializeShader(ID3D11Device * pDev, HWND hwnd, WCHAR * vtx_shader_path, WCHAR * px_shader_path)
{
	HRESULT hr;
	ID3D10Blob* errorMsg = nullptr;
	ID3D10Blob* vtxShaderBuffer = nullptr;
	ID3D10Blob* pxShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC shaderLayouts[2];
	unsigned int nrOfElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	

	//compile vtx shader code
	hr = D3DCompileFromFile(vtx_shader_path,
		nullptr,
		nullptr,
		"light_vs_main",
		"vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		&vtxShaderBuffer,
		&errorMsg);

	if (FAILED(hr))
	{
		if (errorMsg)
		{
			this->OutputShaderErrorMessage(errorMsg, hwnd, vtx_shader_path);
		}
		return false;
	}

	//compile px shader
	hr = D3DCompileFromFile(px_shader_path,
		nullptr,
		nullptr,
		"light_ps_main",
		"ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		&pxShaderBuffer,
		&errorMsg);

	if (FAILED(hr))
	{
		if (errorMsg)
		{
			this->OutputShaderErrorMessage(errorMsg, hwnd, vtx_shader_path);
		}
		return false;
	}

	//create vtx shader
	hr = pDev->CreateVertexShader(vtxShaderBuffer->GetBufferPointer(), vtxShaderBuffer->GetBufferSize(), NULL, &this->_pVertexShader);
	if (FAILED(hr))
		return false;

	//create px shader
	hr = pDev->CreatePixelShader(pxShaderBuffer->GetBufferPointer(), pxShaderBuffer->GetBufferSize(), NULL, &this->_pPixelShader);
	if (FAILED(hr))
		return false;

	//create the vtx input layout desc
	
	shaderLayouts[0].SemanticName = "POSITION";
	shaderLayouts[0].SemanticIndex = 0;
	shaderLayouts[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	shaderLayouts[0].InputSlot = 0;
	shaderLayouts[0].AlignedByteOffset = 0;
	shaderLayouts[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	shaderLayouts[0].InstanceDataStepRate = 0;

	shaderLayouts[1].SemanticName = "TEXCOORD";
	shaderLayouts[1].SemanticIndex = 0;
	shaderLayouts[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	shaderLayouts[1].InputSlot = 0;
	shaderLayouts[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	shaderLayouts[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	shaderLayouts[1].InstanceDataStepRate = 0;

	//count of elements in the layout
	nrOfElements = sizeof(shaderLayouts) / sizeof(shaderLayouts[0]);

	//create the vtx input layout
	hr = pDev->CreateInputLayout(shaderLayouts, nrOfElements, vtxShaderBuffer->GetBufferPointer(), vtxShaderBuffer->GetBufferSize(), &this->_pLayout);

	if (FAILED(hr))
		return false;

	vtxShaderBuffer->Release();
	vtxShaderBuffer = nullptr;
	pxShaderBuffer->Release();
	pxShaderBuffer = nullptr;

	//create sampler desc
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//create texture sampler
	hr = pDev->CreateSamplerState(&samplerDesc, &this->_pSampleState);
	if (FAILED(hr))
		return false;

	//prepare matrix buffer desc
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferStruct);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//create constant buffer
	hr = pDev->CreateBuffer(&matrixBufferDesc, NULL, &this->_pMatrixBuffer);
	if (FAILED(hr))
		return false;

	//prepare light buffer
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferStruct);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	//create light buffer
	hr = pDev->CreateBuffer(&lightBufferDesc, NULL, &this->_pLightBuffer);
	if (FAILED(hr))
		return false;

	//create vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc{ 0, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
	D3D11_SUBRESOURCE_DATA vertexSubresourceData{};

	vertexBufferDesc.ByteWidth = sizeof(Vertex) * (UINT)this->_fullScreenQuad.vertices.size();
	vertexSubresourceData.pSysMem = this->_fullScreenQuad.vertices.data();

	hr = pDev->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &this->_pVertexBuffer);
	if (FAILED(hr))
		return false;

	return true;
}

void LightShader::ReleaseShader()
{
	if (this->_pLayout)
	{
		this->_pLayout->Release();
		this->_pLayout = nullptr;
	}
	if (this->_pLightBuffer)
	{
		this->_pLightBuffer->Release();
		this->_pLightBuffer = nullptr;
	}
	if (this->_pMatrixBuffer)
	{
		this->_pMatrixBuffer->Release();
		this->_pMatrixBuffer = nullptr;
	}
	if (this->_pPixelShader)
	{
		this->_pPixelShader->Release();
		this->_pPixelShader = nullptr;
	}
	if (this->_pSampleState)
	{
		this->_pSampleState->Release();
		this->_pSampleState = nullptr;
	}
	if (this->_pVertexShader)
	{
		this->_pVertexShader->Release();
		this->_pVertexShader = nullptr;
	}
	if (this->_pVertexBuffer)
	{
		this->_pVertexBuffer->Release();
		this->_pVertexBuffer = nullptr;
	}
		
}

void LightShader::OutputShaderErrorMessage(ID3D10Blob * blob, HWND hwnd, WCHAR * fileName)
{
	char* error;
	unsigned int bufferSIze, i;
	ofstream fileOut;

	error = (char*)(blob->GetBufferPointer());
	bufferSIze = (UINT)blob->GetBufferSize();

	fileOut.open("light_shader_error.txt");

	for (i = 0; i < bufferSIze; i++)
	{
		fileOut << error[i];
	}

	fileOut.close();

	blob->Release();
	blob = nullptr;

	MessageBox(hwnd, L"Error compiling light shader. Check light_shader_error.txt", fileName, MB_OK);
}

bool LightShader::SetShaderParameters(ID3D11DeviceContext * pDevCon, XMMATRIX world, XMMATRIX view, XMMATRIX projection, ID3D11ShaderResourceView * colorTexture, ID3D11ShaderResourceView * normalTexture, ID3D11ShaderResourceView* positionTexture, ID3D11Buffer* materialBuffer, XMVECTOR lightDir)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNr;
	MatrixBufferStruct* dataPtrMatrix;
	LightBufferStruct* dataPtrLight;

	//transpose matrices
	world = XMMatrixTranspose(world);
	view = XMMatrixTranspose(view);
	projection = XMMatrixTranspose(projection);

	//map the constant buffer for writing
	hr = pDevCon->Map(this->_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
		return false;

	//get data pointer from buffer
	dataPtrMatrix = (MatrixBufferStruct*)mappedResource.pData;

	//copy matrices
	dataPtrMatrix->world = world;
	dataPtrMatrix->view = view;
	dataPtrMatrix->projection = projection;

	//unmap buffer
	pDevCon->Unmap(this->_pMatrixBuffer, 0);
	//set the constant buffer
	bufferNr = 0;
	pDevCon->VSSetConstantBuffers(bufferNr, 1, &this->_pMatrixBuffer);

	//set texture resources (color and normals) in the pixel shader
	pDevCon->PSSetShaderResources(0, 1, &colorTexture);
	pDevCon->PSSetShaderResources(1, 1, &normalTexture);

	//map constant buffer
	hr = pDevCon->Map(this->_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
		return false;

	//same procedure as above
	dataPtrLight = (LightBufferStruct*)mappedResource.pData;
	dataPtrLight->lightPos = lightDir;
	dataPtrLight->lightColor = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	dataPtrLight->ambientLight = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	pDevCon->Unmap(this->_pLightBuffer, 0);
	bufferNr = 0;
	//set the constant buffer in px shader
	pDevCon->PSSetConstantBuffers(bufferNr++, 1, &this->_pLightBuffer);
	pDevCon->PSSetConstantBuffers(bufferNr, 1, &materialBuffer);

	return true;
}

void LightShader::RenderShader(ID3D11DeviceContext * pDevCon)
{
	//set the layout
	pDevCon->IASetInputLayout(this->_pLayout);
	//set the vertex and pixel shader
	pDevCon->VSSetShader(this->_pVertexShader, NULL, 0);
	pDevCon->PSSetShader(this->_pPixelShader, NULL, 0);
	//set the sampler state
	pDevCon->PSSetSamplers(0, 1, &this->_pSampleState);
	pDevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;
	//set fullscreen quad vertex buffer
	pDevCon->IASetVertexBuffers(0, 1, &this->_pVertexBuffer, &stride, &offset);
	//render the geometry
	pDevCon->Draw((unsigned int)this->_fullScreenQuad.vertices.size(), 0);
}
