#pragma once
#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <vector>
#include <DirectXMath.h>
#include "WICTextureLoader.h"
#include <fstream>
#include "DiamondSqaure.h"

using namespace DirectX;
using namespace std;

class HeightMap
{
public:
	HeightMap();
	~HeightMap();

	bool initialize(ID3D11Device * pDev, HWND hwnd, int mapSize, float offset);
	void Release();
	void Render(ID3D11DeviceContext * pDevCon, XMMATRIX world, XMMATRIX view, XMMATRIX projection);

private:
	struct VertexTypeHeightMap
	{
		XMFLOAT3 vertex;
		XMFLOAT2 tex;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};
	struct MatrixBufferHeightMap
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	//members
	int m_mapSize;  //this is the maps size (the size of one side, nr of vertices is then size times size)
	float m_offset; //this offset determines how far vertices are apart from each other

	vector<float> m_heightValues;
	vector<VertexTypeHeightMap> m_vertices;
	vector<int> m_indices;

	ID3D11Buffer * m_vertexBuffer;
	ID3D11Buffer * m_indexBuffer;
	ID3D11Buffer * m_MatrixBuffer;

	ID3D11InputLayout * m_InputLayout;
	ID3D11VertexShader * m_VertexShader;
	ID3D11PixelShader * m_PixelShader;

	ID3D11Resource * m_textureResource;
	ID3D11Resource * m_normalResource;

	ID3D11ShaderResourceView * m_colorTexture;
	ID3D11ShaderResourceView * m_normalMap;
	ID3D11SamplerState * m_Sampler;

	ID3D11RasterizerState * m_rasterState; //this is soley for wireframe drawing
	D3D11_VIEWPORT m_Viewport;

	bool initShaders(ID3D11Device * pDev, HWND hwnd, WCHAR * vtx_path, WCHAR * px_path);
	bool initTextures(ID3D11Device * pDev, WCHAR * colorTex, WCHAR * normalTex);
	bool initRasterizer(ID3D11Device * pDev);
	bool setShaderParameters(ID3D11DeviceContext * pDevCon, XMMATRIX world, XMMATRIX view, XMMATRIX projection);

	void generateHeightValues();
	bool buildHeightMap(ID3D11Device * pDev);
	void calculateNormalTangentBinormal(int vtx1, int vtx2, int vtx3);

	void outputErrorMessage(ID3D10Blob * error, HWND hwnd, WCHAR * file);

};
#endif // !HEIGHTMAP_H

