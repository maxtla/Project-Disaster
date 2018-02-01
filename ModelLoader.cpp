#include "ModelLoader.h"





ModelLoader::ModelLoader()
{

}


ModelLoader::~ModelLoader()
{
	this->_models.clear();
}

bool ModelLoader::load(ID3D11Device* pDev, char* file_path)
{
	
	FILE *model = nullptr;
	FILE *material = nullptr;
	fopen_s(&model, file_path, "r");
	if (model == nullptr) //make sure the file is open to continue
		return false;

	//create a new model
	Model _tModel;
	int vCount, vtCount, vnCount;
	vCount = vtCount = vnCount = 0;
	
	char lineBuffer[128]; //should be plenty of space
	char materialFile[64]; //64 char is plenty of space, to extract the material file name build a string from index 0 until '\0' is found
	int v1, v2, v3, vt1, vt2, vt3, vn1, vn2, vn3;

	while (fscanf(model, "%s", lineBuffer) != EOF) //read the first word in the line buffer and check if it is not EOF //why does fscan_s not work?
	{
		if (strcmp(lineBuffer, "mtllib") == 0) //check for material file name
		{
			fscanf_s(model, "%s", materialFile, (unsigned int)sizeof(materialFile));
		}
		if (strcmp(lineBuffer, "v") == 0) //check for vertices
		{
			float x, y, z;
			fscanf_s(model, "%f %f %f\n", &x, &y, &z);
			z *= -1.0f;
			_tModel.vertices.push_back(Vertex(x, y, z));
			vCount++;
		}
		if (strcmp(lineBuffer, "vt") == 0) //check for tex coords
		{
			float u, v;
			fscanf_s(model, "%f %f\n", &u, &v);
			v = 1.0f - v;
			_tModel.texCoords.push_back(TexCoord(u, v));
			vtCount++;
		}
		if (strcmp(lineBuffer, "vn") == 0) //check for normals
		{
			float x, y, z;
			fscanf_s(model, "%f %f %f\n", &x, &y, &z);
			z *= -1.0f;
			_tModel.normals.push_back(Normal(x, y, z));
			vnCount++;
		}
		if (strcmp(lineBuffer, "f") == 0) //check for faces
		{
			//this is tricky, to do this we have to find the right format, we can determine the format by checking which counters != 0
			//possibel formats are: f v1 v2 v3 vertex indices
			//						f v1/vt1 v2/vt2 v3/vt3 vertex texture coord. indices
			//						f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 vertex tex coord normal indices
			//						f v1//vn1 v2//vn2 v3//vn3 vertex normal indices (misses tex coords)
			// In the case that no normals were in the file we should use a geometry shader to calculate the normal for a triangle via passthrough, set a flag for geometry shader
			if (vCount != 0 && vtCount != 0 && vnCount != 0)
			{
				// f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 vertex tex coord normal indices
				int result = fscanf_s(model, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);
				if (result != 9)
					return false; //something was missed 
				else
				{
					_tModel.vtxIndices.push_back(v3 - 1);
					_tModel.vtxIndices.push_back(v2 - 1);
					_tModel.vtxIndices.push_back(v1 - 1);

					_tModel.uvIndices.push_back(vt3 - 1);
					_tModel.uvIndices.push_back(vt2 - 1);
					_tModel.uvIndices.push_back(vt1 - 1);

					_tModel.normalIndices.push_back(vn3 - 1);
					_tModel.normalIndices.push_back(vn2 - 1);
					_tModel.normalIndices.push_back(vn1 - 1);
				}
			}
			if (vCount != 0 && vtCount != 0 && vnCount == 0)
			{
				// f v1/vt1 v2/vt2 v3/vt3 vertex texture coord. indices
				int result = fscanf_s(model, "%d/%d %d/%d %d/%d\n", &v1, &vt1, &v2, &vt2, &v3, &vt3);
				if (result != 6)
					return false; //something was missed
				else
				{
					_tModel.vtxIndices.push_back(v1 - 1);
					_tModel.vtxIndices.push_back(v2 - 1);
					_tModel.vtxIndices.push_back(v3 - 1);

					_tModel.uvIndices.push_back(vt1 - 1);
					_tModel.uvIndices.push_back(vt2 - 1);
					_tModel.uvIndices.push_back(vt3 - 1);
				}
			}
			if (vCount != 0 && vtCount == 0 && vnCount != 0)
			{
				// f v1//vn1 v2//vn2 v3//vn3 vertex normal indices (misses tex coords)
				int result = fscanf_s(model, "%d//%d %d//%d %d//%d\n", &v1, &vn1, &v2, &vn2, &v3, &vn3);
				if (result != 6)
					return false; //something was missed
				else
				{
					_tModel.vtxIndices.push_back(v1 - 1);
					_tModel.vtxIndices.push_back(v2 - 1);
					_tModel.vtxIndices.push_back(v3 - 1);

					_tModel.normalIndices.push_back(vn1 - 1);
					_tModel.normalIndices.push_back(vn2 - 1);
					_tModel.normalIndices.push_back(vn3 - 1);
				}
			}
			if (vCount != 0 && vtCount == 0 && vnCount == 0)
			{
				// f v1 v2 v3 vertex indices
				int result = fscanf_s(model, "%d %d %d\n", &v1, &v2, &v3);
				if (result != 3)
					return false; //something was missed
				else
				{
					_tModel.vtxIndices.push_back(v1 - 1);
					_tModel.vtxIndices.push_back(v2 - 1);
					_tModel.vtxIndices.push_back(v3 - 1);
				}
			}
		}
	}
	fclose(model);
	//parse material file path correctly
	string m_file_path = materialFile;
	m_file_path = "Assets//" + m_file_path;
	//open material file
	fopen_s(&material, m_file_path.c_str(), "r");
	//local variables
	char texture_file[64];
	//now parse the material file
	while (fscanf(material, "%s", lineBuffer) != EOF)
	{
		if (strcmp(lineBuffer, "Ns") == 0)
		{
			fscanf_s(material, "%f", &_tModel.material.specular_exponent);
		}
		if (strcmp(lineBuffer, "Ka") == 0)
		{
			fscanf_s(material, "%f", &_tModel.material.ambient);
		}
		if (strcmp(lineBuffer, "Kd") == 0)
		{
			fscanf_s(material, "%f", &_tModel.material.diffuse);
		}
		if (strcmp(lineBuffer, "Ks") == 0)
		{
			fscanf_s(material, "%f", &_tModel.material.specular);
		}
		/*if (strcmp(lineBuffer, "d") == 0)
		{
			fscanf_s(material, "%f", &_tModel.material.d_factor);
		}*/
		if (strcmp(lineBuffer, "map_Kd") == 0) //check for texture file name
		{
			fscanf_s(material, "%s", texture_file, (unsigned int)sizeof(texture_file));
		}

	}
	fclose(material);
	//set flags for this model 
	if (vnCount == 0)
	{
		_tModel.NO_NORMALS = 1; //0 == false 1 == true
	}
	//create the texture path
	string texture_path = texture_file;
	texture_path = "Assets//" + texture_path;
	//load in texture for this model
	if (!_tModel.loadTexture(pDev, texture_path))
		return false;
	//initialize the vertex buffer
	_tModel.initializeBuffer(pDev);
	//add the model to our model vector
	this->_models.push_back(_tModel);

	return true;
}

int ModelLoader::size() const
{
	return (int)this->_models.size();
}

Model ModelLoader::getModel(int index)
{
	return this->_models[index];
}

void ModelLoader::Release()
{
	for (int i = 0; i < _models.size(); i++)
	{
		this->_models[i].Release();
	}
}
