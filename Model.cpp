#include "Model.h"



Model::Model()
{
	NO_NORMALS = 0;
}


Model::~Model()
{
	vtxIndices.clear();
	uvIndices.clear();
	normalIndices.clear();
	vertices.clear();
	texCoords.clear();
	normals.clear();
	this->_modelResource->Release();
	this->_modelResource = nullptr;
	this->_modelTextureView->Release();
	this->_modelTextureView = nullptr;
	this->_modelSamplerState->Release();
	this->_modelSamplerState = nullptr;
}

bool Model::loadTexture(ID3D11Device * pDev, string texture)
{
	HRESULT hr;
	wstring widestr = wstring(texture.begin(), texture.end()); //convert string object to wstring object to get the const wchar_t*

	hr = CreateWICTextureFromFile(pDev, widestr.c_str(), &this->_modelResource, &this->_modelTextureView);
	if (FAILED(hr))
		return false;

	//create the sampler
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//create the sampler state
	hr = pDev->CreateSamplerState(&sampDesc, &this->_modelSamplerState);
	if (FAILED(hr))
		return false;


	return true;
}

ID3D11ShaderResourceView * Model::getTexture()
{
	return this->_modelTextureView;
}

ID3D11Resource * Model::getResource()
{
	return this->_modelResource;
}

ID3D11SamplerState * Model::getSampler()
{
	return this->_modelSamplerState;
}
