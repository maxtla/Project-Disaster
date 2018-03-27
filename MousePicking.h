#ifndef MOUSEPICKING_H
#define MOUSEPICKING_H

#include<d3d11.h>
#include<DirectXMath.h>

class MousePicking
{
private:
	// window variables
	int mWidth;
	int mHeight;

	DirectX::XMVECTOR shootRay(float mouseX, float mouseY, DirectX::XMVECTOR cameraCoords, DirectX::XMMATRIX proj, DirectX::XMMATRIX view, DirectX::XMMATRIX world);

public:
	MousePicking();
	virtual~MousePicking();

	void initialize(int width, int height);
	DirectX::XMVECTOR detectInput(DirectX::XMFLOAT2 mouseCoords, DirectX::XMVECTOR cameraCoords, DirectX::XMMATRIX proj, DirectX::XMMATRIX view, DirectX::XMMATRIX world);
};

#endif