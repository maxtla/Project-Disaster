#pragma once
#ifndef _MODEL_H
#define _MODEL_H
#include <vector>
#include <d3d11.h>
#include "WICTextureLoader.h"
#include <comdef.h>
#include <stdio.h>
#include <DirectXMath.h>
#include <chrono>
#include <wrl\client.h>

using namespace std;
using namespace DirectX;
using namespace std::chrono;


struct Vertex
{
	float x, y, z;
	XMFLOAT3 tangent, binormal;
	Vertex(float _x, float _y, float _z, XMFLOAT3 t, XMFLOAT3 b)
	{
		x = _x;
		y = _y;
		z = _z;
		tangent = t;
		binormal = b;
	}
};

struct TexCoord
{
	float u, v;
	TexCoord(float _u, float _v)
	{
		u = _u;
		v = _v;
	}
};

struct Normal
{
	float x, y, z;
	Normal(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
};

struct VerticesUVsNormals
{
	VerticesUVsNormals() {}
	VerticesUVsNormals(float _x, float _y, float _z, float _u, float _v, float _a, float _b, float _c,
		float _tx, float _ty, float _tz, float _bx, float _by, float _bz)
	{
		x = _x;
		y = _y;
		z = _z;
		u = _u;
		v = _v;
		a = _a;
		b = _b;
		c = _c;
		tx = _tx;
		ty = _ty;
		tz = _tz;
		bx = _bx;
		by = _by;
		bz = _bz;
	}
	float x, y, z;  //vertex
	float u, v;		//tex coords
	float a, b, c;  //normal
	float tx, ty, tz; //tangent
	float bx, by, bz; //binormal
};

__declspec(align(16))
struct Material
{
	float ambient;//ambient color , we need to inverse this in the px shader = 1 - ambient
	float diffuse; //diffuse color
	float specular;//specular color
	float specular_exponent;
	//float d_factor; //transparency factor 1.0 means fully opaque, Tr = 1 - d_factor;
	int hasNormMap;
};

class Model
{
private:
	ID3D11Buffer *pVertexBuffer;
	ID3D11ShaderResourceView* _modelTextureView;
	ID3D11ShaderResourceView* _modelNormalMap;
	ID3D11Resource* _modelNormalResource;
	ID3D11Resource* _modelTextureResource;
	ID3D11SamplerState* _modelSamplerState;
	ID3D11Buffer* _pMaterialBuffer;
	VerticesUVsNormals* _pVerticesUvNormArr;
	XMMATRIX world;
public:
	Model();
	~Model();
	//containers
	vector<unsigned int> vtxIndices;
	vector<unsigned int> uvIndices;
	vector<unsigned int> normalIndices;
	vector<unsigned int > tangBinormIndices;
	vector<Vertex> vertices;
	vector<TexCoord> texCoords;
	vector<Normal> normals;
	vector<Normal> tangents;
	vector<Normal> binormals;
	Material material;
	int NO_NORMALS;
	bool m_hasNormalMap;
	//methods
	bool loadTexture(ID3D11Device* pDev, string texture);
	bool initializeBuffer(ID3D11Device* pDev);
	ID3D11ShaderResourceView* getTexture();
	ID3D11ShaderResourceView* getNormalMap();
	bool hasNormalMap();
	ID3D11Resource* getResource();
	ID3D11SamplerState* getSampler();
	VerticesUVsNormals* getData();
	int getVertexCount();
	ID3D11Buffer * getMaterialBuffer();
	void Render(ID3D11DeviceContext *pDevCon);
	void Release();
	XMMATRIX getWorld();
	void setWorld(XMMATRIX world);

};
#endif // !_MODEL_H

