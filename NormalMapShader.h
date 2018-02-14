#pragma once
#ifndef _NORMALMAPSHADER_H
#define _NORMALMAPSHADER_H
#include "Application.h"
#include <fstream>

using namespace DirectX;

class NormalMapShader
{
private: 
	__declspec(align(16))
		struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	ID3D11VertexShader* _pVertexShader;
	ID3D11PixelShader* _pPixelShader;
	ID3D11InputLayout* _pInputLayout;
	ID3D11SamplerState* _pSamplerState;
	ID3D11Buffer* _pMatrixBuffer;


	bool initializeShader(ID3D11Device* pDev, HWND hwnd, WCHAR* vertexFile, WCHAR* fragmentFile);
	void ReleaseShader();
	void outputErrorMessage(ID3D10Blob* blob, HWND hwnd, WCHAR* file);
	bool setShaderParameters(ID3D11DeviceContext* pDevCon, XMMATRIX world, XMMATRIX view, XMMATRIX projection, ID3D11ShaderResourceView* pTexture, ID3D11ShaderResourceView* pNormalMap, bool renderNormalMap, ID3D11Buffer* materialBuffer, ID3D11Buffer* lightBuffer);
	void renderShader(ID3D11DeviceContext* pDevCon, int iCount, ID3D11SamplerState* texSampler);

public:
	NormalMapShader();
	~NormalMapShader();
	bool initialize(ID3D11Device* pDev, HWND hwnd);
	void Release();
	bool render(ID3D11DeviceContext* pDevCon, int index, XMMATRIX world, XMMATRIX view, XMMATRIX projection, ID3D11ShaderResourceView* pTexture, ID3D11ShaderResourceView* pNormalMap, bool renderNormalMap, ID3D11SamplerState* texSampler, ID3D11Buffer * materialBuffer, ID3D11Buffer * lightBuffer);
};
#endif // !_NORMALMAPSHADER_H

