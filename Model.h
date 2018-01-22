#pragma once
#ifndef _MODEL_H
#define _MODEL_H
#include <vector>

using namespace std;

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

class Model
{
private:

public:
	Model();
	~Model();
	bool loadTexture(char* texture);
	//containers
	vector<unsigned int> vtxIndices;
	vector<unsigned int> uvIndices;
	vector<unsigned int> normalIndices;
	vector<Vertex> vertices;
	vector<TexCoord> texCoords;
	vector<Normal> normals;

};
#endif // !_MODEL_H

