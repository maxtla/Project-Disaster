#pragma once
#ifndef _MODELLOADER_H
#define _MODELLOADER_H
#include "Model.h"
#include <string>
#include <fstream>

using namespace std;

class ModelLoader
{
private:
	vector<Model> _models;
	void _calculateTangentBinormal(Model &model, int v1, int v2, int v3, int vt1, int vt2, int vt3);
public:
	ModelLoader();
	~ModelLoader();
	bool load(ID3D11Device* pDev, char* file_path, XMMATRIX world);
	int size() const;
	Model getModel(int index);
	void Release();
	void update(XMMATRIX world, int index);
};
#endif // !_MODELLOADER_H

