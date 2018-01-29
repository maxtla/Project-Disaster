#pragma once
#ifndef _DEFERREDBUFFER_H
#define _DEFERREDBUFFER_H
//includes
#include <d3d11.h>
//macros
#define ARR_SIZE 2

class DeferredBuffer
{
public:
	DeferredBuffer();
	//DefferedBuffer(const DefferedBuffer&);
	~DeferredBuffer();

	bool initialize(ID3D11Device* pDev, int texWidth, int texHeight, float farPlane, float nearPlane); //farPlane == screen depth, nearPlane == screen near
	void Release();
	void setRenderTargets(ID3D11DeviceContext* pDevCon);
	void clearRenderTargets(ID3D11DeviceContext* pDevCon, float r, float g, float b, float a);

	ID3D11ShaderResourceView* getShaderResourceView(int view);

private:
	int _texWidth, _texHeight;
	//render targets are put in an array now
	ID3D11Texture2D* _RTTArr[ARR_SIZE];  //render target texture array
	ID3D11RenderTargetView* _RTVArr[ARR_SIZE]; //render target view array
	ID3D11ShaderResourceView* _SRVArr[ARR_SIZE]; //shader resource view array
	ID3D11Texture2D* _DSBuffer;
	ID3D11DepthStencilView* _DSV;
	D3D11_VIEWPORT _viewport;
};

#endif // !_DEFFEREDBUFFER_H
