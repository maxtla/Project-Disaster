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
	vector<Model*> _models;
public:
	ModelLoader();
	~ModelLoader();
	bool load(char* name, char* texture);
	int size() const;
	Model* getModel(int index);
};
#endif // !_MODELLOADER_H

