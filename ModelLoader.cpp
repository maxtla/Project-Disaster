#include "ModelLoader.h"



ModelLoader::ModelLoader()
{
}


ModelLoader::~ModelLoader()
{
	for (auto obj : this->_models)
	{
		delete obj;
	}
}

bool ModelLoader::load(char* name, char* texture)
{
	FILE *model = nullptr;
	fopen_s(&model, name, "r");
	if (model == nullptr) //make sure the file is open to continue
		return false;

	//create a new model
	Model* _tModel = new Model();
	char lineBuffer[128]; //should be plenty of space
	while (fscanf(model, "%s", lineBuffer) != EOF) //read the first word in the line buffer and check if it is not EOF //why does fscan_s not work?
	{
		if (strcmp(lineBuffer, "v") == 0) //check for vertices
		{
			float x, y, z;
			fscanf_s(model, "%f %f %f\n", &x, &y, &z);
			_tModel->vertices.push_back(Vertex(x, y, z));
		}
		if (strcmp(lineBuffer, "vt") == 0) //check for tex coords
		{
			float u, v;
			fscanf_s(model, "%f %f\n", &u, &v);
			_tModel->texCoords.push_back(TexCoord(u, v));
		}
		if (strcmp(lineBuffer, "vn") == 0) //check for normals
		{
			float x, y, z;
			fscanf_s(model, "%f %f %f\n", &x, &y, &z);
			_tModel->normals.push_back(Normal(x, y, z));
		}
		if (strcmp(lineBuffer, "f") == 0) //check for faces
		{
			//this is tricky
		}
	}

	return true;
}

int ModelLoader::size() const
{
	return (int)this->_models.size();
}

Model * ModelLoader::getModel(int index)
{
	return this->_models[index];
}
