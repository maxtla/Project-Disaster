#pragma once
#ifndef DEPTHSHADER_H
#define DEPTHSHADER_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

using namespace std;
using namespace DirectX;

#define SHADOWMAP_WIDTH 1280
#define SHADOWMAP_HEIGHT 720

class DepthShader
{
private:
	__declspec(align(16))
	struct MatrixBufferStruct
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	//members
	ID3D11VertexShader * m_VertexShader;
	ID3D11PixelShader * m_PixelShader;
	ID3D11InputLayout * m_InputLayout;
	ID3D11Buffer * m_MatrixBuffer;
	ID3D11Texture2D * m_depthBufferTexture;
	ID3D11DepthStencilView * m_DSV;
	ID3D11ShaderResourceView * m_SRV;
	ID3D11RasterizerState * m_RenderState;
	D3D11_VIEWPORT m_Viewport;

	//methods
	bool initializeShader(ID3D11Device * pDev, HWND hwnd, WCHAR* vtx_path, WCHAR* px_path);
	bool initializeDepthBuffer(ID3D11Device * pDev);
	void outputErrorMessage(ID3D10Blob * error, HWND hwnd, WCHAR * file);
	bool setShaderParameters(ID3D11DeviceContext * pDevCon, XMMATRIX world, XMMATRIX view, XMMATRIX projection);
	void RenderShader(ID3D11DeviceContext * pDevCon, int vertexCount);
public:
	//methods
	DepthShader();
	~DepthShader();
	bool Initialize(ID3D11Device * pDev, HWND hwnd);
	void Release();
	bool Render(ID3D11DeviceContext * pDevCon, int vertexCount, XMMATRIX world, XMMATRIX view, XMMATRIX projection);
	ID3D11ShaderResourceView * getDepthBufferSRV() { return this->m_SRV; }
	bool setRenderTarget(ID3D11DeviceContext * pDevCon);
};
#endif // !DEPTHSHADER_H

