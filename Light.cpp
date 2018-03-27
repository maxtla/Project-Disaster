#include "Light.h"



void Light::makeMatrices(LightType &light)
{
	this->lightView = XMMatrixLookAtLH(light.lightPosOrDir, this->focusPoint, XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
	this->lightProjection = XMMatrixPerspectiveFovLH(XM_PI * 0.45f, ((float)1280) / 720, 0.01f, 20.0f);
}

Light::Light()
{
	this->m_lightBuffer = nullptr;
}


Light::~Light()
{
}

bool Light::initLightBuffer(ID3D11Device * pDev, int type, XMVECTOR posOrDir, XMVECTOR color, XMVECTOR ambient, XMVECTOR focusPoint)
{
	LightType light;
	this->type = type;
	this->focusPoint = focusPoint;

	light.lightPosOrDir = posOrDir;
	light.lightColor = color;
	light.ambientLight = ambient;

	this->makeMatrices(light);

	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_SUBRESOURCE_DATA lightData;

	ZeroMemory(&lightBufferDesc, sizeof(D3D11_BUFFER_DESC));

	lightBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	lightBufferDesc.ByteWidth = (unsigned int)(sizeof(LightType));
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = 0;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	lightData.pSysMem = &light;
	lightData.SysMemPitch = 0;
	lightData.SysMemSlicePitch = 0;

	HRESULT hr = pDev->CreateBuffer(&lightBufferDesc, &lightData, &this->m_lightBuffer);
	if (FAILED(hr))
	{
		/*const TCHAR * err = _com_error(hr).ErrorMessage();

		OutputDebugString(err);*/
		return false;
	}

	return true;
}

void Light::Release()
{
	if (this->m_lightBuffer)
		this->m_lightBuffer->Release();

	this->m_lightBuffer = nullptr;
}
