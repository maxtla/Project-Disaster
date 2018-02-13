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
	materialBufferDesc.Usage = D3D11_USAGE_DEFAULT; //this means it can only be read by the GPU and can not be accessed by the CPU. After initialization the data can not be changed. Which is OK for now
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
	this->material.hasNormMap = 0;
	NO_NORMALS = 0;
	this->_pVerticesUvNormArr = nullptr;
	this->_modelNormalResource = nullptr;
	this->_modelTextureResource = nullptr;
	this->_modelSamplerState = nullptr;
	this->_modelTextureView = nullptr;
	this->_pMaterialBuffer = nullptr;
	this->m_hasNormalMap = false;
	this->_modelNormalMap = nullptr;
}


Model::~Model()
{
	
}

bool Model::loadTexture(ID3D11Device * pDev, string texture)
{
	HRESULT hr;
	wstring widestr = wstring(texture.begin(), texture.end()); //convert string object to wstring object to get the const wchar_t*

	if (this->_modelTextureView == nullptr)
		hr = CreateWICTextureFromFile(pDev, widestr.c_str(), &this->_modelTextureResource, &this->_modelTextureView);
	else if (this->_modelTextureView != nullptr && this->m_hasNormalMap && this->_modelNormalMap == nullptr)
	{
		Microsoft::WRL::ComPtr<ID3D11Resource> res;
		hr = CreateWICTextureFromFile(pDev, widestr.c_str(), &this->_modelNormalResource, &this->_modelNormalMap);
	/*	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
		hr = res.As(&tex);
		D3D11_TEXTURE2D_DESC desc;
		tex->GetDesc(&desc);*/
	}

	if (FAILED(hr))
		return false;
	if (this->_modelSamplerState == nullptr)
	{
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
	}


	return true;
}

ID3D11ShaderResourceView * Model::getTexture()
{
	return this->_modelTextureView;
}

ID3D11ShaderResourceView * Model::getNormalMap()
{
	return this->_modelNormalMap;
}

bool Model::hasNormalMap()
{
	return this->m_hasNormalMap;
}

ID3D11Resource * Model::getResource()
{
	return this->_modelTextureResource;
}

ID3D11SamplerState * Model::getSampler()
{
	return this->_modelSamplerState;
}

VerticesUVsNormals * Model::getData()
{
	float x, y, z, u, v, a, b, c, tx, ty, tz, bx, by, bz;
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

			tx = this->vertices[this->vtxIndices[i]].tangent.x;
			ty = this->vertices[this->vtxIndices[i]].tangent.y;
			tz = this->vertices[this->vtxIndices[i]].tangent.z;

			bx = this->vertices[this->vtxIndices[i]].binormal.x;
			by = this->vertices[this->vtxIndices[i]].binormal.y;
			bz = this->vertices[this->vtxIndices[i]].binormal.z;

			this->_pVerticesUvNormArr[i] = VerticesUVsNormals(x, y, z, u, v, a, b, c, tx, ty, tz, bx, by, bx);
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
	if (this->_modelTextureResource)
	{
		this->_modelTextureResource->Release();
		this->_modelTextureResource = nullptr;
	}
	if (this->_modelNormalResource)
	{
		this->_modelNormalResource->Release();
		this->_modelNormalResource = nullptr;
	}
	if (this->_modelTextureView)
	{
		this->_modelTextureView->Release();
		this->_modelTextureView = nullptr;
	}
	if (this->_modelSamplerState)
	{
		this->_modelSamplerState->Release();
		this->_modelSamplerState = nullptr;
	}
	if (this->pVertexBuffer)
	{
		this->pVertexBuffer->Release();
		this->pVertexBuffer = nullptr;
	}
	if (this->_pMaterialBuffer)
	{
		this->_pMaterialBuffer->Release();
		this->_pMaterialBuffer = nullptr;
	}
	if (this->_modelNormalMap)
	{
		this->_modelNormalMap->Release();
		this->_modelNormalMap = nullptr;
	}
	if (this->_pVerticesUvNormArr)
		delete[] this->_pVerticesUvNormArr;
}

XMMATRIX Model::
getWorld()
{
	return this->world;
}

void Model::setWorld(XMMATRIX world)
{
	this->world = world;
}
