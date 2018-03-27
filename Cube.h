#ifndef CUBE_H
#define CUBE_H

#include <DirectXCollision.h>
#include <vector>
#include "Application.h"

class Cube
{
private:
	__declspec(align(16))
	struct BoxVertices
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

	std::vector<DirectX::BoundingBox> m_boxes;
	BoxVertices* m_boxVertices;
	int vertexCount;
	bool boxHit;
	int hitBoxId;

public:
	Cube();
	virtual~Cube();
	bool initialize(Application* pApp, ID3D11Buffer** cubeBuffer, int nrOfBoxes = 2);
	void Release();
	int getHitBoxId() const;
	bool getBoxHit() const;
	UINT32 getStride() const;
	bool intersectBoundingBox(DirectX::XMVECTOR origin, DirectX::XMVECTOR dir);
	void update(Application* pApp, ID3D11Buffer* &vertexBuffer, bool boxGotHit, int boxId = 0);
	void Render(ID3D11DeviceContext* pDevCon);

};

#endif