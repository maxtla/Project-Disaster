#include "MousePickingShader.h"

bool MousePickingShader::initializeShader(ID3D11Device* pDev, HWND hwnd, ID3D11Buffer* &vertexBuffer)
{
	//collect local members here
	HRESULT hr;
	ID3D10Blob* errorMsg = NULL;
	ID3D10Blob* vertexShaderBuffer = NULL;
	ID3D10Blob* pixelShaderBuffer = NULL;
	D3D11_BUFFER_DESC matrixBufferDesc;

	if (vertexBuffer != nullptr)
		this->m_vertexBuffer = vertexBuffer;

	// compile vertex shader from file
	hr = D3DCompileFromFile(
		L"Shaders//mousePickingVS.hlsl",
		nullptr,
		nullptr,
		"mousepicking_vs_main",
		"vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		&vertexShaderBuffer,
		nullptr
	);
	pDev->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &this->m_vertexShader);

	if (FAILED(hr))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	hr = pDev->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &this->m_vertexLayout);

	if (FAILED(hr))
	{
		return false;
	}

	// compile fragment shader from file
	hr = D3DCompileFromFile(
		L"Shaders//mousePickingPS.hlsl",
		nullptr,
		nullptr,
		"mousepicking_ps_main",
		"ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		&pixelShaderBuffer,
		&errorMsg);

	hr = pDev->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &this->_pPixelShader);
	if (FAILED(hr))
		return false;

	//clean up a little
	vertexShaderBuffer->Release();
	vertexShaderBuffer = NULL;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = NULL;

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

	return true;
}

void MousePickingShader::ReleaseShader()
{
	if (this->_pMatrixBuffer)
	{
		this->_pMatrixBuffer->Release();
		this->_pMatrixBuffer = NULL;
	}
	if (this->m_vertexShader)
	{
		this->m_vertexShader->Release();
		this->m_vertexShader = NULL;
	}
	if (this->_pPixelShader)
	{
		this->_pPixelShader->Release();
		this->_pPixelShader = NULL;
	}
}

void MousePickingShader::outputErrorMessage(ID3D10Blob * blob, HWND hwnd, WCHAR * file)
{
	char* compileError;
	unsigned int bufferSize;
	ofstream fOut;

	//pointer to the error message text buffer
	compileError = (char*)(blob->GetBufferPointer());
	//get message size
	bufferSize = (unsigned int)blob->GetBufferSize();
	//open an error text file and write to it
	fOut.open("mousePickingShader_error.txt");
	for (unsigned int i = 0; i < bufferSize; i++)
	{
		fOut << compileError[i];
	}

	fOut.close();

	blob->Release();
	blob = NULL;

	MessageBox(hwnd, L"Error compiling shader.  Check mousePickingShader_error.txt", file, MB_OK);
}

bool MousePickingShader::setShaderParameters(ID3D11DeviceContext* pDevCon, XMMATRIX world, XMMATRIX view, XMMATRIX projection)
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

	return true;
}

void MousePickingShader::renderShader(ID3D11DeviceContext* pDevCon, UINT stride)
{
	//set vertex input layout
	pDevCon->IASetInputLayout(this->m_vertexLayout);
	//set the vertex and pixel shader
	pDevCon->VSSetShader(this->m_vertexShader, NULL, 0);
	pDevCon->PSSetShader(this->_pPixelShader, NULL, 0);

	unsigned int offset = 0;

	pDevCon->IASetVertexBuffers(0, 1, &this->m_vertexBuffer, &stride, &offset);
	pDevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	//render the geometry
	pDevCon->Draw(48, 0);
}

MousePickingShader::MousePickingShader()
{
	this->_pMatrixBuffer = NULL;
	this->_pPixelShader = NULL;
}

MousePickingShader::~MousePickingShader()
{
}

ID3D11Buffer *& MousePickingShader::getVertexBuffer()
{
	return this->m_vertexBuffer;
}

bool MousePickingShader::initialize(ID3D11Device* pDev, HWND hwnd, ID3D11Buffer* &vertexBuffer)
{
	bool result;
	//load in the vertex and pixel shader files
	result = this->initializeShader(pDev, hwnd, vertexBuffer);
	if (!result)
		return false;

	return true;
}

bool MousePickingShader::render(ID3D11DeviceContext* pDevCon, UINT stride, XMMATRIX world, XMMATRIX view, XMMATRIX projection)
{
	bool result;
	//set parameters used for rendering
	result = this->setShaderParameters(pDevCon, world, view, projection);
	if (!result)
		return false;

	//render the prepared buffers
	this->renderShader(pDevCon, stride);

	return true;
}

void MousePickingShader::release()
{
	this->ReleaseShader();
}
