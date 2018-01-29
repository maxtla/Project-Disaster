#pragma once
#ifndef _DEFERREDSHADER_H
#define _DEFERREDSHADER_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <d3dcompiler.h>

using namespace std;
using namespace DirectX;

class DeferredShader
{
private:
	//members
	struct MatrixBufferStruct
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	ID3D11VertexShader* _pVertexShader;
	ID3D11PixelShader* _pPixelShader;
	ID3D11InputLayout* _pInputLayout;
	ID3D11SamplerState* _pSamplerState;
	ID3D11Buffer* _pMatrixBuffer;

	//methods
	bool initializeShader(ID3D11Device* pDev, HWND hwnd, WCHAR* vertexFile, WCHAR* fragmentFile);
	void ReleaseShader();
	void outputErrorMessage(ID3D10Blob* blob, HWND hwnd, WCHAR* file);
	bool setShaderParameters(ID3D11DeviceContext* pDevCon, XMMATRIX world, XMMATRIX view, XMMATRIX projection, ID3D11ShaderResourceView* pSRV);
	void renderShader(ID3D11DeviceContext* pDevCon, int iCount, ID3D11SamplerState* texSampler);
public:
	DeferredShader();
	~DeferredShader();
	bool initialize(ID3D11Device* pDev, HWND hwnd);
	void Release();
	bool render(ID3D11DeviceContext* pDevCon, int index, XMMATRIX world, XMMATRIX view, XMMATRIX projection, ID3D11ShaderResourceView* pSRV, ID3D11SamplerState* texSampler);
};

#endif // !_DEFERREDSHADER_H
