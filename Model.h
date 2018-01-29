#pragma once
#ifndef _MODEL_H
#define _MODEL_H
#include <vector>
#include <d3d11.h>
#include "WICTextureLoader.h"

using namespace std;
using namespace DirectX;

struct Vertex
{
	float x, y, z;
	Vertex(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
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
	VerticesUVsNormals(float _x, float _y, float _z, float _u, float _v, float _a, float _b, float _c)
	{
		x = _x;
		y = _y;
		z = _z;
		u = _u;
		v = _v;
		a = _a;
		b = _b;
		c = _c;
	}
	float x, y, z;  //vertix
	float u, v;		//tex coords
	float a, b, c;  //normal
};

struct Material
{
	float ambient[3] = { 0 };
	float diffuse[3] = { 0 };
	float specular[3] = { 0 };
	float specular_exponent;
	float d_factor; //transparency factor 1.0 means fully opaque, Tr = 1 - d_factor;

};

class Model
{
private:
	ID3D11Buffer *pVertexBuffer;
	ID3D11ShaderResourceView* _modelTextureView;
	ID3D11Resource* _modelResource;
	ID3D11SamplerState* _modelSamplerState;
	VerticesUVsNormals* _pVerticesUvNormArr;
public:
	Model();
	~Model();
	//containers
	vector<unsigned int> vtxIndices;
	vector<unsigned int> uvIndices;
	vector<unsigned int> normalIndices;
	vector<Vertex> vertices;
	vector<TexCoord> texCoords;
	vector<Normal> normals;
	Material material;
	int NO_NORMALS;
	//methods
	bool loadTexture(ID3D11Device* pDev, string texture);
	bool initializeBuffer(ID3D11Device* pDev);
	ID3D11ShaderResourceView* getTexture();
	ID3D11Resource* getResource();
	ID3D11SamplerState* getSampler();
	VerticesUVsNormals* getData();
	int getVertexCount();
	void Render(ID3D11DeviceContext *pDevCon);
	void Release();

};
#endif // !_MODEL_H

