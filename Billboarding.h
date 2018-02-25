#pragma once
#ifndef BILLBOARDING_H
#define BILLBOARDING_H

#include"d3d11.h"
#include"d3dcompiler.h"
#include <DirectXMath.h>
#include "WICTextureLoader.h"
#include<fstream>
#include"HeightMap.h"

class Billboarding
{
private:
	struct VertexQuad
	{
		float x, y, z;
		float u, v;
	};

	struct MatrixBufferStruct
	{
		XMMATRIX worldPos;
		XMMATRIX view;
		XMMATRIX projection;
	};
	struct GeometryBufferStruct
	{
		XMVECTOR camPos;
	};

	ID3D11VertexShader* _pVertexShader;
	ID3D11PixelShader* _pPixelShader;
	ID3D11GeometryShader* _pGeometryShader;
	ID3D11InputLayout* _pInputLayout;
	ID3D11SamplerState* _pSamplerState;
	ID3D11Buffer* _pMatrixBuffer;
	ID3D11Buffer* _pGeometryBuffer;

	bool initializeShader(ID3D11Device * pDev, HWND hwnd, WCHAR* geometryFile, WCHAR* fragmentFile, WCHAR* vertexFile);
	void outputErrorMessage(ID3D10Blob* blob, HWND hwnd, WCHAR* file);
	bool setShaderParameters(ID3D11DeviceContext* pDevCon, XMMATRIX world, XMMATRIX view, XMMATRIX proj, ID3D11ShaderResourceView* pTexture, XMVECTOR camPos);
	void renderShader(ID3D11DeviceContext* pDevCon, int iCount, ID3D11SamplerState* texSampler);
	void releaseShader();
public:
	Billboarding();
	~Billboarding();

	bool initialize(ID3D11Device* pDev, HWND hwnd);
	void Release();
	void createTriangleData();
	bool render(ID3D11DeviceContext* pDevCon, int index, XMMATRIX world, XMMATRIX view, XMMATRIX proj, ID3D11ShaderResourceView* pTexture, ID3D11SamplerState* texSampler, XMVECTOR camPos);
};

#endif // !BILLBOARDING_H

