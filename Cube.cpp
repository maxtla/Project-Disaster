#include "Cube.h"

Cube::Cube()
{
	this->m_boxVertices = nullptr;
	this->boxHit = false;
	this->hitBoxId = -1;
	this->vertexCount = 0;
}

Cube::~Cube()
{
}

bool Cube::initialize(Application* pApp, ID3D11Buffer** cubeBuffer, int nrOfBoxes)
{
	DirectX::XMFLOAT3 corners[DirectX::BoundingBox::CORNER_COUNT];
	this->m_boxVertices = new BoxVertices[nrOfBoxes * 24];
	this->vertexCount = nrOfBoxes * 24;

	for (int i = 0; i < nrOfBoxes; i++)
	{
		this->m_boxes.push_back(DirectX::BoundingBox(DirectX::XMFLOAT3(float(i * 1), float(i * 1), float(i * 4)), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)));
		this->m_boxes[i].GetCorners(corners);

		this->m_boxVertices[i * 24 + 0].position = corners[0];
		this->m_boxVertices[i * 24 + 1].position = corners[1];
		this->m_boxVertices[i * 24 + 2].position = corners[1];
		this->m_boxVertices[i * 24 + 3].position = corners[2];
		this->m_boxVertices[i * 24 + 4].position = corners[2];
		this->m_boxVertices[i * 24 + 5].position = corners[3];
		this->m_boxVertices[i * 24 + 6].position = corners[3];
		this->m_boxVertices[i * 24 + 7].position = corners[0];

		this->m_boxVertices[i * 24 + 8].position = corners[0];
		this->m_boxVertices[i * 24 + 9].position = corners[4];
		this->m_boxVertices[i * 24 + 10].position = corners[1];
		this->m_boxVertices[i * 24 + 11].position = corners[5];
		this->m_boxVertices[i * 24 + 12].position = corners[2];
		this->m_boxVertices[i * 24 + 13].position = corners[6];
		this->m_boxVertices[i * 24 + 14].position = corners[3];
		this->m_boxVertices[i * 24 + 15].position = corners[7];

		this->m_boxVertices[i * 24 + 16].position = corners[4];
		this->m_boxVertices[i * 24 + 17].position = corners[5];
		this->m_boxVertices[i * 24 + 18].position = corners[5];
		this->m_boxVertices[i * 24 + 19].position = corners[6];
		this->m_boxVertices[i * 24 + 20].position = corners[6];
		this->m_boxVertices[i * 24 + 21].position = corners[7];
		this->m_boxVertices[i * 24 + 22].position = corners[7];
		this->m_boxVertices[i * 24 + 23].position = corners[4];
	}
	for (int i = 0; i < nrOfBoxes * 24; i++)
		this->m_boxVertices[i].color = DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

	HRESULT hr;

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(BoxVertices) * vertexCount;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = this->m_boxVertices;
	hr = pApp->pDev->CreateBuffer(&bufferDesc, &data, cubeBuffer);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void Cube::Release()
{
	this->m_boxes.clear();
	delete[] this->m_boxVertices;
}

int Cube::getHitBoxId() const
{
	return this->hitBoxId;
}

bool Cube::getBoxHit() const
{
	return boxHit;
}

UINT32 Cube::getStride() const
{
	UINT32 vertexSize = sizeof(BoxVertices);
	return vertexSize;
}

bool Cube::intersectBoundingBox(DirectX::XMVECTOR origin, DirectX::XMVECTOR dir)
{
	float intersectionDistance;
	float t = FLT_MAX;
	bool toReturn = false;

	for (int i = 0; i < this->m_boxes.size(); i++)
	{
		if (this->m_boxes[i].Intersects(origin, dir, intersectionDistance))
		{
			if (intersectionDistance < t)
			{
				t = intersectionDistance;
				this->hitBoxId = i;
			}
			if (!toReturn)
				toReturn = true;
		}
	}

	this->boxHit = toReturn;

	return toReturn;
}

void Cube::update(Application* pApp, ID3D11Buffer* &vertexBuffer, bool boxGotHit, int boxId)
{
	if (boxGotHit)
	{
		// 24 lines
		for (int i = 0; i < 24; i++)
			this->m_boxVertices[(this->hitBoxId * 24) + i].color = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

		D3D11_BUFFER_DESC bufferDesc;
		vertexBuffer->GetDesc(&bufferDesc);
		vertexBuffer->Release();

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = this->m_boxVertices;
		pApp->pDev->CreateBuffer(&bufferDesc, &data, &vertexBuffer);
		this->boxHit = false;
	}
	else
	{
		for (int i = 0; i < 24; i++)
			this->m_boxVertices[(boxId * 24) + i].color = DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

		vertexBuffer->Release();

		D3D11_BUFFER_DESC bufferDesc;
		memset(&bufferDesc, 0, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(BoxVertices) * vertexCount;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = this->m_boxVertices;
		pApp->pDev->CreateBuffer(&bufferDesc, &data, &vertexBuffer);
	}
}

void Cube::Render(ID3D11DeviceContext* pDevCon)
{
}