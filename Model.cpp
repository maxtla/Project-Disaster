#include "Model.h"



Model::Model()
{
}


Model::~Model()
{
	vtxIndices.clear();
	uvIndices.clear();
	normalIndices.clear();
	vertices.clear();
	texCoords.clear();
	normals.clear();
}
