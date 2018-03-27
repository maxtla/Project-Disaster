#include "ModelLoader.h"





void ModelLoader::_calculateTangentBinormal(Model & model, int v1, int v2, int v3, int vt1, int vt2, int vt3, int vn)
{
	XMFLOAT3 vtx1, vtx2, vtx3;
	XMFLOAT2 uv1, uv2, uv3;
	XMFLOAT3 normal;

	//get vertices
	vtx1.x = model.vertices[v1 - 1].x;
	vtx1.y = model.vertices[v1 - 1].y;
	vtx1.z = model.vertices[v1 - 1].z;

	vtx2.x = model.vertices[v2 - 1].x;
	vtx2.y = model.vertices[v2 - 1].y;
	vtx2.z = model.vertices[v2 - 1].z;

	vtx3.x = model.vertices[v3 - 1].x;
	vtx3.y = model.vertices[v3 - 1].y;
	vtx3.z = model.vertices[v3 - 1].z;

	//get UVs
	uv1.x = model.texCoords[vt1 - 1].u;
	uv1.y = model.texCoords[vt1 - 1].v;

	uv2.x = model.texCoords[vt2 - 1].u;
	uv2.y = model.texCoords[vt2 - 1].v;

	uv3.x = model.texCoords[vt3 - 1].u;
	uv3.y = model.texCoords[vt3 - 1].v;

	//get normal
	normal.x = model.normals[vn - 1].x;
	normal.y = model.normals[vn - 1].y;
	normal.z = model.normals[vn - 1].z;

	//load the data into XMVECTOR's so we can use DirectXMath functions/operators for calculations
	XMVECTOR vertex0, vertex1, vertex2, tex0, tex1, tex2, normal0;
	vertex0 = XMLoadFloat3(&vtx1);
	vertex1 = XMLoadFloat3(&vtx2);
	vertex2 = XMLoadFloat3(&vtx3);
	tex0 = XMLoadFloat2(&uv1);
	tex1 = XMLoadFloat2(&uv2);
	tex2 = XMLoadFloat2(&uv3);
	normal0 = XMLoadFloat3(&normal);

	//do the math
	XMVECTOR e1 = vertex1 - vertex0;
	XMVECTOR e2 = vertex2 - vertex0;
	XMVECTOR deltaUV1 = tex1 - tex0;
	XMVECTOR deltaUV2 = tex2 - tex0;

	XMVECTOR tangent;
	XMVECTOR binormal;

	float r = (deltaUV1.m128_f32[0] * deltaUV2.m128_f32[1] - deltaUV1.m128_f32[1] * deltaUV2.m128_f32[0]);
	if (fabsf(r) < 1e-6f)
	{
		// Equal to zero (almost) means the surface lies flat on its back
		tangent = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		binormal = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		r = 1.0f / r;
		tangent = (e1 * deltaUV2.m128_f32[1] - e2 * deltaUV1.m128_f32[1]) * r;
		binormal = (e2 * deltaUV1.m128_f32[0] - e1 * deltaUV2.m128_f32[0]) * r;

		tangent = XMVector3Normalize(tangent);
		binormal = XMVector3Normalize(binormal);
	}

	// As the bitangent equals to the cross product between the normal and the tangent running along the surface, calculate it
	XMVECTOR bitangent = XMVector3Cross(normal0, tangent);

	// Since we don't know if we must negate it, compare it with our computed one above
	float crossinv = (XMVector3Dot(bitangent, binormal).m128_f32[0] < 0.0f) ? -1.0f : 1.0f;
	bitangent *= crossinv;

	//finally add the tangent and binormal to each vertex
	XMStoreFloat3(&model.vertices[v1 - 1].tangent, tangent);
	XMStoreFloat3(&model.vertices[v2 - 1].tangent, tangent);
	XMStoreFloat3(&model.vertices[v3 - 1].tangent, tangent);

	XMStoreFloat3(&model.vertices[v1 - 1].binormal, bitangent);
	XMStoreFloat3(&model.vertices[v2 - 1].binormal, bitangent);
	XMStoreFloat3(&model.vertices[v3 - 1].binormal, bitangent);

}

ModelLoader::ModelLoader()
{
}


ModelLoader::~ModelLoader()
{
	this->_models.clear();
}

bool ModelLoader::load(ID3D11Device* pDev, char* file_path, XMMATRIX world)
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
			_tModel.vertices.push_back(Vertex(x, y, z, XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f)));
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
					//before we push back the indices, lets calculate the tangent and binormal for the vertices of this face
					this->_calculateTangentBinormal(_tModel, v1, v2, v3, vt1, vt2, vt3, vn1);

					//now push back the indices
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
	std::fclose(model);
	//parse material file path correctly
	string m_file_path = materialFile;
	m_file_path = "Assets//" + m_file_path;
	//open material file
	fopen_s(&material, m_file_path.c_str(), "r");
	//local variables
	char texture_file[64];
	char bump_file[64];
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
		if (strcmp(lineBuffer, "map_Bump") == 0)
		{
			_tModel.material.hasNormMap = 1;
			_tModel.m_hasNormalMap = true;
			fscanf_s(material, "%s", bump_file, (unsigned int)sizeof(bump_file));
		}

	}
	std::fclose(material);
	//set flags for this model 
	if (vnCount == 0)
	{
		_tModel.NO_NORMALS = 1; //0 == false 1 == true
	}
	//create the texture path
	string texture_path = texture_file;
	texture_path = "Assets//" + texture_path;
	//load in texture(s) for this model
	if (!_tModel.loadTexture(pDev, texture_path))
		return false;
	if (_tModel.m_hasNormalMap)
	{
		string bump_path = bump_file;
		bump_path = "Assets//" + bump_path;
		if (!_tModel.loadTexture(pDev, bump_path))
			return false;
	}
	//initialize the vertex buffer
	_tModel.initializeBuffer(pDev);
	//set the world matrix for this model
	_tModel.setWorld(world);
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

void ModelLoader::update(XMMATRIX world, int index)
{
	this->_models[index].setWorld(world);
}
