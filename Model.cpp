#include "Model.h"



bool Model::initializeBuffer(ID3D11Device * pDev)
{
	VerticesUVsNormals * vertices = nullptr;
	vertices = this->getData();
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_BUFFER_DESC materialBufferDesc;
	D3D11_SUBRESOURCE_DATA materialData;
	HRESULT hr;

	if (vertices == nullptr)
		return false;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = (unsigned int)(sizeof(VerticesUVsNormals) * this->vtxIndices.size());
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//create the vertex buffer
	hr = pDev->CreateBuffer(&vertexBufferDesc, &vertexData, &this->pVertexBuffer);
	if (FAILED(hr))
		return false;
	//prepare material const buffer
	materialBufferDesc.Usage = D3D11_USAGE_IMMUTABLE; //this means it can only be read by the GPU and can not be accessed by the CPU. After initialization the data can not be changed. Which is OK for now
	materialBufferDesc.ByteWidth = (unsigned int)(sizeof(Material));
	materialBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialBufferDesc.CPUAccessFlags = 0;
	materialBufferDesc.MiscFlags = 0;
	materialBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the material data.

	materialData.pSysMem = &material;
	materialData.SysMemPitch = 0;
	materialData.SysMemSlicePitch = 0;

	//create material const buffer
	hr = pDev->CreateBuffer(&materialBufferDesc, &materialData, &this->_pMaterialBuffer);
	if (FAILED(hr))
	{
		const TCHAR * err = _com_error(hr).ErrorMessage();

		OutputDebugString(err);
		return false;
	}

	return true;
}

Model::Model()
{
	NO_NORMALS = 0;
	this->_pVerticesUvNormArr = nullptr;
	this->_modelResource = nullptr;
	this->_modelSamplerState = nullptr;
	this->_modelTextureView = nullptr;
	this->_pMaterialBuffer = nullptr;
}


Model::~Model()
{
	
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

VerticesUVsNormals * Model::getData()
{
	float x, y, z, u, v, a, b, c;
	if (this->_pVerticesUvNormArr == nullptr)
	{
		//create the array and put in data in the correct order
		this->_pVerticesUvNormArr = new VerticesUVsNormals[this->vtxIndices.size()];
		for (size_t i = 0; i < this->vtxIndices.size(); i++)
		{
			x = this->vertices[this->vtxIndices[i]].x;
			y = this->vertices[this->vtxIndices[i]].y;
			z = this->vertices[this->vtxIndices[i]].z;

			u = this->texCoords[this->uvIndices[i]].u;
			v = this->texCoords[this->uvIndices[i]].v;

			a = this->normals[this->normalIndices[i]].x;
			b = this->normals[this->normalIndices[i]].y;
			c = this->normals[this->normalIndices[i]].z;

			this->_pVerticesUvNormArr[i] = VerticesUVsNormals(x, y, z, u, v, a, b, c);
		}
	}

	return this->_pVerticesUvNormArr;
}

int Model::getVertexCount()
{
	return (int)this->vtxIndices.size();
}

ID3D11Buffer * Model::getMaterialBuffer()
{
	return this->_pMaterialBuffer;
}

void Model::Render(ID3D11DeviceContext * pDevCon)
{
	unsigned int stride = sizeof(VerticesUVsNormals);
	unsigned int offset = 0;

	pDevCon->IASetVertexBuffers(0, 1, &this->pVertexBuffer, &stride, &offset);
	pDevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	return;
}

void Model::Release()
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
	this->pVertexBuffer->Release();
	this->pVertexBuffer = nullptr;
	if (this->_pVerticesUvNormArr)
		delete[] this->_pVerticesUvNormArr;
}
