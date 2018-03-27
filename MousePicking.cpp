#include "MousePicking.h"

DirectX::XMVECTOR MousePicking::shootRay(float mouseX, float mouseY, DirectX::XMVECTOR cameraCoords, DirectX::XMMATRIX proj, DirectX::XMMATRIX view, DirectX::XMMATRIX world)
{
	DirectX::XMVECTOR mouseCoordsNear = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(mouseX, mouseY, 0.0f));
	DirectX::XMVECTOR mouseCoordsFar = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(mouseX, mouseY, 1.0f));
	DirectX::XMVECTOR orig = DirectX::XMVector3Unproject(mouseCoordsNear, 0.0f, 0.0f, (float)this->mWidth, (float)this->mHeight, 0.0f, 1.0f, proj, view, world);
	DirectX::XMVECTOR dest = DirectX::XMVector3Unproject(mouseCoordsFar, 0.0f, 0.0f, (float)this->mWidth, (float)this->mHeight, 0.0f, 1.0f, proj, view, world);
	DirectX::XMVECTOR direction = DirectX::XMVectorSubtract(dest, orig);
	direction = DirectX::XMVector3Normalize(direction);

	return direction;
}

MousePicking::MousePicking()
{
}

MousePicking::~MousePicking()
{
}

void MousePicking::initialize(int width, int height)
{
	this->mWidth = width;
	this->mHeight = height;
}

DirectX::XMVECTOR MousePicking::detectInput(DirectX::XMFLOAT2 mouseCoords, DirectX::XMVECTOR cameraCoords, DirectX::XMMATRIX proj, DirectX::XMMATRIX view, DirectX::XMMATRIX world)
{
	return this->shootRay(mouseCoords.x, mouseCoords.y, cameraCoords, proj, view, world);
}
