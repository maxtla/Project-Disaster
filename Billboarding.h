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
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	ID3D11VertexShader* _pVertexShader;
	ID3D11PixelShader* _pPixelShader;
	ID3D11GeometryShader * _pGeometryShader;
	ID3D11InputLayout* _pInputLayout;
	ID3D11SamplerState* _pSamplerState;
	ID3D11Buffer* _pMatrixBuffer;

	void outputErrorMessage(ID3D10Blob* blob, HWND hwnd, WCHAR* file);
public:
	Billboarding();
	~Billboarding();

	bool initialize(ID3D11Device * pDev, HWND hwnd, WCHAR* geometryFile, WCHAR* fragmentFile, WCHAR* vertexFile);
	void Release();
	void createTriangleData();
};

#endif // !BILLBOARDING_H

