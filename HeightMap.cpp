#include "HeightMap.h"



HeightMap::HeightMap()
{
}


HeightMap::~HeightMap()
{
}

bool HeightMap::initialize(ID3D11Device * pDev, HWND hwnd, int mapSize, float offset)
{
	this->m_mapSize = mapSize;
	this->m_offset = offset;

	if (!this->initShaders(pDev, hwnd, L"Shaders//heightMapVertex.hlsl", L"Shaders//heightMapPixel.hlsl"))
		return false;

	if (!this->initTextures(pDev, L"Assets//heightMapColors.tif", L"Assets//heightMapNormals.tif"))
		return false;

	if (!this->initRasterizer(pDev))
		return false;

	this->generateHeightValues();
	if (!this->buildHeightMap(pDev))
		return false;

	return true;
}

void HeightMap::Release()
{
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}
	if (m_InputLayout)
	{
		m_InputLayout->Release();
		m_InputLayout = nullptr;
	}
	if (m_VertexShader)
	{
		m_VertexShader->Release();
		m_VertexShader = nullptr;
	}
	if (m_PixelShader)
	{
		m_PixelShader->Release();
		m_PixelShader = nullptr;
	}
	if (m_textureResource)
	{
		m_textureResource->Release();
		m_textureResource = nullptr;
	}
	if (m_normalResource)
	{
		m_normalResource->Release();
		m_normalResource = nullptr;
	}
	if (m_colorTexture)
	{
		m_colorTexture->Release();
		m_colorTexture = nullptr;
	}
	if (m_normalMap)
	{
		m_normalMap->Release();
		m_normalMap = nullptr;
	}
	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = nullptr;
	}
	if (m_Sampler)
	{
		m_Sampler->Release();
		m_Sampler = nullptr;
	}

	m_heightValues.clear();
	m_vertices.clear();
	m_indices.clear();

}

void HeightMap::Render(ID3D11DeviceContext * pDevCon, XMMATRIX world, XMMATRIX view, XMMATRIX projection)
{
	this->setShaderParameters(pDevCon, world, view, projection);
	pDevCon->DrawIndexed((unsigned int)this->m_indices.size(), 0, 0);
}

bool HeightMap::initShaders(ID3D11Device * pDev, HWND hwnd, WCHAR * vtx_path, WCHAR * px_path)
{
	//collect local members here
	HRESULT hr;
	ID3D10Blob* errorMsg = NULL;
	ID3D10Blob* vertexShaderBuffer = NULL;
	ID3D10Blob* pixelShaderBuffer = NULL;
	D3D11_INPUT_ELEMENT_DESC layoutDescriptions[5];
	unsigned int numOfElements;

	//compile vertex shader from code file
	hr = D3DCompileFromFile(vtx_path,
		nullptr,
		nullptr,
		"heightmap_vs_main",
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
		"heightmap_ps_main",
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
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferHeightMap);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	hr = pDev->CreateBuffer(&matrixBufferDesc, NULL, &this->m_MatrixBuffer);
	if (FAILED(hr))
		return false;

	return true;
}

bool HeightMap::initTextures(ID3D11Device * pDev, WCHAR * colorTex, WCHAR * normalTex)
{
	HRESULT hr;
	if (m_colorTexture == nullptr)
	{
		hr = CreateWICTextureFromFile(pDev, colorTex, &this->m_textureResource, &this->m_colorTexture);
		if (FAILED(hr))
			return false;
	}
	if (m_normalMap == nullptr)
	{
		hr = CreateWICTextureFromFile(pDev, normalTex, &this->m_normalResource, &this->m_normalMap);
		if (FAILED(hr))
			return false;
	}

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 1;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = pDev->CreateSamplerState(&samplerDesc, &this->m_Sampler);
	if (FAILED(hr))
		return false;
	return true;
}

bool HeightMap::initRasterizer(ID3D11Device * pDev)
{
	HRESULT hr;
	D3D11_RASTERIZER_DESC pRSDesc;
	ZeroMemory(&pRSDesc, sizeof(D3D11_RASTERIZER_DESC));
	pRSDesc.FillMode = D3D11_FILL_WIREFRAME;
	pRSDesc.CullMode = D3D11_CULL_NONE;
	pRSDesc.DepthClipEnable = true;

	hr = pDev->CreateRasterizerState(&pRSDesc, &this->m_rasterState);
	if (FAILED(hr))
		return false;

	m_Viewport.Height = 720.f;
	m_Viewport.Width = 1280.f;
	m_Viewport.MaxDepth = 1.0f;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.TopLeftX = 0.f;
	m_Viewport.TopLeftY = 0.f;

	return true;
}

bool HeightMap::setShaderParameters(ID3D11DeviceContext * pDevCon, XMMATRIX world, XMMATRIX view, XMMATRIX projection)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	unsigned int bufferNr;
	MatrixBufferHeightMap* dataPtr;

	//set the vertex and pixel shader
	pDevCon->VSSetShader(this->m_VertexShader, NULL, 0);
	pDevCon->PSSetShader(this->m_PixelShader, NULL, 0);

	//set the vertex and index buffer and topology for rendering
	unsigned int stride = sizeof(VertexTypeHeightMap);
	unsigned int offset = 0;

	pDevCon->IASetInputLayout(this->m_InputLayout);
	pDevCon->IASetVertexBuffers(0, 1, &this->m_vertexBuffer, &stride, &offset);
	pDevCon->IASetIndexBuffer(this->m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//set the rasterizer stage and viewport
	//pDevCon->RSSetState(this->m_rasterState);
	pDevCon->RSSetViewports(1, &this->m_Viewport);

	//transpose matrices since we have not specified anything in the compiler options
	world = XMMatrixTranspose(world);
	view = XMMatrixTranspose(view);
	projection = XMMatrixTranspose(projection);

	//lock the constant buffer for writing
	hr = pDevCon->Map(this->m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(hr))
		return false;

	//get the data pointer of the constant buffer
	dataPtr = (MatrixBufferHeightMap*)mappedSubresource.pData;
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
	//set shader resources
	pDevCon->PSSetShaderResources(0, 1, &this->m_colorTexture);
	pDevCon->PSSetShaderResources(1, 1, &this->m_normalMap);
	pDevCon->PSSetSamplers(0, 1, &this->m_Sampler);

	return true;
}

void HeightMap::generateHeightValues()
{
	DiamondSqaure ds;
	this->m_heightValues = ds.createDiamondSquare(this->m_mapSize, this->m_mapSize - 1, 1.0f);
}

bool HeightMap::buildHeightMap(ID3D11Device * pDev)
{
	//generate the vertices
	VertexTypeHeightMap v;
	for (int z = 0; z < m_mapSize; z++)
	{
		for (int x = 0; x < this->m_mapSize; x++)
		{
			int index = x + (m_mapSize * z);
			v.vertex = XMFLOAT3((float)x * m_offset, m_heightValues[index], (float)z * m_offset);
			//v.vertex = XMFLOAT3((float)x * m_offset, 0.0f, (float)z * m_offset);
			v.tex = XMFLOAT2(0.0f, 0.0f); //need to calculate these later
			v.normal = XMFLOAT3(0.0f, 0.0f, 0.0f); //calculate this later
			v.tangent = v.binormal = v.normal; //also have to be calculated later

			m_vertices.push_back(v);
		}
	}

	//generate the indices
	for (int z = 0; z < m_mapSize - 1; z++)
	{
		for (int x = 0; x < m_mapSize - 1; x++) {

			int v1, v2, v3, v4, v5, v6;

			v1 = (z + 1)*m_mapSize + x;
			v2 = (z*m_mapSize + x + 1);
			v3 = z*m_mapSize + x;

			v4 = z*m_mapSize + x + 1;
			v5 = (z + 1)*m_mapSize + x;
			v6 = (z + 1)*m_mapSize + x + 1;

			//set the UV coordinats
			m_vertices[v1].tex = XMFLOAT2(0.0f, 0.0f);
			m_vertices[v2].tex = XMFLOAT2(1.0f, 1.0f);
			m_vertices[v3].tex = XMFLOAT2(0.0f, 1.0f);

			m_vertices[v4].tex = XMFLOAT2(1.0f, 1.0f);
			m_vertices[v5].tex = XMFLOAT2(0.0f, 0.0f);
			m_vertices[v6].tex = XMFLOAT2(1.0f, 0.0f);

			//calculate the normal, tangent and binormal
			calculateNormalTangentBinormal(v1, v2, v3);
			calculateNormalTangentBinormal(v4, v5, v6);

			//push back the indices
			m_indices.push_back(v1);
			m_indices.push_back(v2);
			m_indices.push_back(v3);

			m_indices.push_back(v4);
			m_indices.push_back(v5);
			m_indices.push_back(v6);
		}
	}

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT hr;

	// set up the description of the static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexTypeHeightMap) * (unsigned int)m_vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// assign the subresource structure a pointer to the vertex data
	vertexData.pSysMem = m_vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// create vertex buffer
	hr = pDev->CreateBuffer(&vertexBufferDesc, &vertexData, &this->m_vertexBuffer);
	if (FAILED(hr))
		return false;

	// set up the description of the static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (unsigned int)m_indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// assign the subresource structure a pointer t o the index data
	indexData.pSysMem = m_indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// create index buffer
	hr = pDev->CreateBuffer(&indexBufferDesc, &indexData, &this->m_indexBuffer);
	if (FAILED(hr))
		return false;

	return true;

}

void HeightMap::calculateNormalTangentBinormal(int v1, int v2, int v3)
{
	XMFLOAT3 vtx1, vtx2, vtx3;
	XMFLOAT2 uv1, uv2, uv3;
	XMFLOAT3 normal;

	vtx1 = m_vertices[v1].vertex;
	vtx2 = m_vertices[v2].vertex;
	vtx3 = m_vertices[v3].vertex;

	uv1 = m_vertices[v1].tex;
	uv2 = m_vertices[v2].tex;
	uv3 = m_vertices[v3].tex;

	XMVECTOR edge1 = XMLoadFloat3(&vtx2) - XMLoadFloat3(&vtx1);
	XMVECTOR edge2 = XMLoadFloat3(&vtx3) - XMLoadFloat3(&vtx1);

	XMVECTOR n = XMVector3Normalize(XMVector3Cross(edge1, edge2));

	XMStoreFloat3(&normal, n);

	m_vertices[v1].normal = normal;
	m_vertices[v2].normal = normal;
	m_vertices[v3].normal = normal;

	//load the data into XMVECTOR's so we can use DirectXMath functions/operators for calculations
	XMVECTOR vertex0, vertex1, vertex2, tex0, tex1, tex2, normal0;
	vertex0 = XMLoadFloat3(&vtx1);
	vertex1 = XMLoadFloat3(&vtx2);
	vertex2 = XMLoadFloat3(&vtx3);
	tex0 = XMLoadFloat2(&uv1);
	tex1 = XMLoadFloat2(&uv2);
	tex2 = XMLoadFloat2(&uv3);
	normal0 = XMLoadFloat3(&normal);

	//do the math
	XMVECTOR e1 = vertex1 - vertex0;
	XMVECTOR e2 = vertex2 - vertex0;
	XMVECTOR deltaUV1 = tex1 - tex0;
	XMVECTOR deltaUV2 = tex2 - tex0;

	XMVECTOR tangent;
	XMVECTOR binormal;

	float r = (deltaUV1.m128_f32[0] * deltaUV2.m128_f32[1] - deltaUV1.m128_f32[1] * deltaUV2.m128_f32[0]);
	if (fabsf(r) < 1e-6f)
	{
		// Equal to zero (almost) means the surface lies flat on its back
		tangent = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		binormal = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		r = 1.0f / r;
		tangent = (e1 * deltaUV2.m128_f32[1] - e2 * deltaUV1.m128_f32[1]) * r;
		binormal = (e2 * deltaUV1.m128_f32[0] - e1 * deltaUV2.m128_f32[0]) * r;

		tangent = XMVector3Normalize(tangent);
		binormal = XMVector3Normalize(binormal);
	}

	// As the bitangent equals to the cross product between the normal and the tangent running along the surface, calculate it
	XMVECTOR bitangent = XMVector3Cross(normal0, tangent);

	// Since we don't know if we must negate it, compare it with our computed one above
	float crossinv = (XMVector3Dot(bitangent, binormal).m128_f32[0] < 0.0f) ? -1.0f : 1.0f;
	bitangent *= crossinv;

	XMStoreFloat3(&m_vertices[v1].tangent, tangent);
	XMStoreFloat3(&m_vertices[v2].tangent, tangent);
	XMStoreFloat3(&m_vertices[v3].tangent, tangent);

	XMStoreFloat3(&m_vertices[v1].binormal, bitangent);
	XMStoreFloat3(&m_vertices[v2].binormal, bitangent);
	XMStoreFloat3(&m_vertices[v3].binormal, bitangent);
}

void HeightMap::outputErrorMessage(ID3D10Blob * error, HWND hwnd, WCHAR * file)
{
	char* compileError;
	unsigned int bufferSize;
	ofstream fOut;

	//pointer to the error message text buffer
	compileError = (char*)(error->GetBufferPointer());
	//get message size
	bufferSize = (unsigned int)error->GetBufferSize();
	//open an error text file and write to it
	fOut.open("heightMapShader_error.txt");
	for (unsigned int i = 0; i < bufferSize; i++)
	{
		fOut << compileError[i];
	}

	fOut.close();

	error->Release();
	error = NULL;

	MessageBox(hwnd, L"Error compiling shader.  Check heightMapShader_error.txt", file, MB_OK);
	return;
}