#pragma once
#ifndef SHADOWMAPSHADER_H
#define SHADOWMAPSHADER_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

using namespace std;
using namespace DirectX;

class ShadowMapShader
{
private:
	__declspec(align(16))
	struct Matrices
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};
	//members
	ID3D11VertexShader * m_VertexShader;
	ID3D11PixelShader * m_PixelShader;
	ID3D11InputLayout * m_InputLayout;
	ID3D11SamplerState * m_WrapSampler;
	ID3D11SamplerState * m_ComparisonSampler;
	ID3D11Buffer * m_MatrixBuffer;

	//methods
	bool initializeShader(ID3D11Device * pDev, HWND hwnd, WCHAR* vtx_path, WCHAR* px_path);
	void outputErrorMessage(ID3D10Blob * error, HWND hwnd, WCHAR * file);
	bool setShaderParameters(ID3D11DeviceContext * pDevCon, XMMATRIX world, XMMATRIX view, XMMATRIX projection,
								XMMATRIX lightView, XMMATRIX lightProjection, ID3D11ShaderResourceView* modelTexture, 
									ID3D11ShaderResourceView* shadowMap);
	void RenderShader(ID3D11DeviceContext * pDevCon, int vertexCount);
public:
	ShadowMapShader();
	~ShadowMapShader();
	bool Initialize(ID3D11Device * pDev, HWND hwnd);
	void Release();
	bool Render(ID3D11DeviceContext * pDevCon, int vertexCount, XMMATRIX world, XMMATRIX view, XMMATRIX projection,
		XMMATRIX lightView, XMMATRIX lightProjection, ID3D11ShaderResourceView * modelTexture, ID3D11ShaderResourceView * shadowMap);
};
#endif // !SHADOWMAPSHADER_H

