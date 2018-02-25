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

bool Billboarding::initialize(ID3D11Device * pDev, HWND hwnd, WCHAR* geometryFile, WCHAR* fragmentFile, WCHAR* vertexFile)
{
	HRESULT hr;
	ID3D10Blob* errorMsg = NULL;
	ID3D10Blob* geometryShaderBuffer = NULL;
	ID3D10Blob* vertexShaderBuffer = NULL;
	ID3D10Blob* pixelShaderBuffer = NULL;
	D3D11_INPUT_ELEMENT_DESC layoutDescriptions[5];
	unsigned int numOfElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;

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

	return false;
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
