#include "Sphere.h"



Sphere::Sphere()
{
	this->m_modelLoader = nullptr;
	this->m_sphereBounds = nullptr;
}


Sphere::~Sphere()
{
}

bool Sphere::initialize(ID3D11Device *pDev, Movement *inputHandler)
{
	this->inputHandler = inputHandler;
	if (m_modelLoader == nullptr)
		m_modelLoader = new ModelLoader();

	if (m_sphereBounds == nullptr)
		m_sphereBounds = new BoundingSphere(XMFLOAT3(1.0f, 0.0f, 1.0f), 1.0f);

	XMMATRIX world = XMMatrixTranslation(m_sphereBounds->Center.x, m_sphereBounds->Center.y, m_sphereBounds->Center.z);
	if (!m_modelLoader->load(pDev, "Assets//PebbleSphere.obj", world))
		return false;

	return true;
}

void Sphere::Release()
{
	if (m_sphereBounds)
		delete m_sphereBounds;

	if (m_modelLoader)
		m_modelLoader->Release();

	m_indexBufferList.clear();
}

void Sphere::Render(ID3D11DeviceContext * pDevCon)
{
	
	//render the model
	Model m = m_modelLoader->getModel(0);
	ID3D11ShaderResourceView * tex = m.getTexture();
	ID3D11ShaderResourceView * nor = m.getNormalMap();

	pDevCon->PSSetShaderResources(0, 1, &tex);
	pDevCon->PSSetShaderResources(1, 1, &nor);

	m.Render(pDevCon);

	pDevCon->Draw(m.getVertexCount(), 0);
}

bool Sphere::intersectBoundingBox(XMFLOAT3 center, float radius, vector<int> indices)
{
	//build a bounding box from the given values
	BoundingBox t_BB;
	t_BB.Center = center;
	t_BB.Extents = XMFLOAT3(radius, radius, radius);

	//save the Y value of the spehere
	float recoveryY = m_sphereBounds->Center.y;
	//set sphere Y coordinant to 0
	m_sphereBounds->Center.y = 0.0f;
	//test sphere against the BB
	if (m_sphereBounds->Intersects(t_BB))
	{
		if (!indices.empty())
		{
			m_indexBufferList.push_back(indices);
		}

		m_sphereBounds->Center.y = recoveryY; //reset the sphere Y value
		return true;
	}
	m_sphereBounds->Center.y = recoveryY; //reset the sphere Y value
	return false;
}

void Sphere::update(vector<VertexTypeHeightMap> vertices)
{
	XMMATRIX world;
	float averageHeight;
	int heightCount;
	XMFLOAT3 v1, v2, v3;
	float h1, h2, h3;
	float s_y;
	//save the sphere height value, then set it to 0
	s_y = m_sphereBounds->Center.y;
	m_sphereBounds->Center.y = 0.f;
	heightCount = 0;
	averageHeight = 0.f;

	for (size_t i = 0; i < m_indexBufferList.size(); i++)
	{
		for (size_t k = 0; k < m_indexBufferList[i].size(); k += 3)
		{
			//take a triangle
			v1 = vertices[m_indexBufferList[i][k]].vertex;
			v2 = vertices[m_indexBufferList[i][k + 1]].vertex;
			v3 = vertices[m_indexBufferList[i][k + 2]].vertex;

			//temporary save the height values
			h1 = v1.y;
			h2 = v2.y;
			h3 = v3.y;

			//set the height values of the vertices to 0
			v1.y = v2.y = v3.y = 0.f;

			//make intersection test if the sphere intersect with the triangle
			if (m_sphereBounds->Intersects(XMLoadFloat3(&v1), XMLoadFloat3(&v2), XMLoadFloat3(&v3)))
			{
				//if it intersects calculate the average height of this triangle and addit to the total average
				float localAverage = 0.f;
				localAverage = (h1 + h2 + h3) / 3.0f;
				averageHeight += localAverage;
				heightCount++;
			}
		}
	}
	//clear memory
	m_indexBufferList.clear();
	//now check if the heightcount is greater than 0, if so set the new height for the sphere and translate it there
	if (heightCount > 0)
	{
		m_sphereBounds->Center.y = (averageHeight / (float)heightCount) - 0.2f;
	}
	else
		m_sphereBounds->Center.y = s_y;

	world = XMMatrixTranslationFromVector(XMLoadFloat3(&m_sphereBounds->Center));

	//set the new world matrix for the model
	m_modelLoader->update(world, 0);
}

void Sphere::move()
{
	inputHandler->moveSphere(m_sphereBounds->Center);
}

XMMATRIX Sphere::getWorld()
{
	return m_modelLoader->getModel(0).getWorld();
}
