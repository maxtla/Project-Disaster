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
public:
	ModelLoader();
	~ModelLoader();
	bool load(ID3D11Device* pDev, char* file_path);
	int size() const;
	Model getModel(int index);
	void Release();
};
#endif // !_MODELLOADER_H

