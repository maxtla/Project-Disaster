#pragma once
#ifndef  _LIGHTSHADER_H
#define _LIGHTSHADER_H
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <d3dcompiler.h>
#include <vector>

using namespace std;
using namespace DirectX;

class LightShader
{
private:
	struct MatrixBufferStruct
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	struct LightBufferStruct
	{
		XMVECTOR lightDir;
	};
	struct Vertex
	{
		float position[3];
		float normal[3];
		float tex[2];
	};
	struct FullScreenQuad
	{
		vector<Vertex> vertices;
	};
	// private members
	ID3D11VertexShader* _pVertexShader;
	ID3D11PixelShader* _pPixelShader;
	ID3D11InputLayout* _pLayout;
	ID3D11SamplerState* _pSampleState;
	ID3D11Buffer* _pMatrixBuffer;
	ID3D11Buffer* _pLightBuffer;
	ID3D11Buffer* _pVertexBuffer;
	ID3D11Buffer* _pIndexBuffer;
	FullScreenQuad _fullScreenQuad;
	// private methods
	void initFullScreenQuad();
	bool InitializeShader(ID3D11Device* pDev, HWND hwnd, WCHAR* vtx_shader_path, WCHAR* px_shader_path);
	void ReleaseShader();
	void OutputShaderErrorMessage(ID3D10Blob* blob, HWND hwnd, WCHAR* fileName);
	bool SetShaderParameters(ID3D11DeviceContext* pDevCon, XMMATRIX world, XMMATRIX view, XMMATRIX projection, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, XMVECTOR lightDir);
	void RenderShader(ID3D11DeviceContext* pDevCon, int vertexCount);
public:
	LightShader();
	~LightShader();
	bool initialize(ID3D11Device* pDev, HWND hwnd);
	void Release();
	bool Render(ID3D11DeviceContext *pDevCon, int, XMMATRIX world, XMMATRIX view, XMMATRIX projection, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, XMVECTOR lightDir);
};

#endif // ! _LIGHTSHADER_H
