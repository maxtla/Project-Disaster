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
	ID3D11ShaderResourceView* _modelTextureView;
	ID3D11Resource* _modelResource;
	ID3D11SamplerState* _modelSamplerState;
public:
	Model();
	~Model();
	bool loadTexture(ID3D11Device* pDev, string texture);
	//containers
	vector<unsigned int> vtxIndices;
	vector<unsigned int> uvIndices;
	vector<unsigned int> normalIndices;
	vector<Vertex> vertices;
	vector<TexCoord> texCoords;
	vector<Normal> normals;
	Material material;
	int NO_NORMALS;
	ID3D11ShaderResourceView* getTexture();
	ID3D11Resource* getResource();
	ID3D11SamplerState* getSampler();

};
#endif // !_MODEL_H

