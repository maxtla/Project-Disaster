#include "Movement.h"



Movement::Movement()
{
	camPosition = XMVectorSet(0.0f,2.0f,-1.0f,0.0f);
	camTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	camUp = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	//view = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -3.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
}

Movement::~Movement()
{
}

void Movement::initialize(HWND hwnd)
{
	mKeyboard = make_unique<Keyboard>();
	mMouse = make_unique<Mouse>();

	mMouse->SetWindow(hwnd);
	this->startState = mMouse->GetState();
}

void Movement::updateCamera(XMMATRIX &view)
{
	float y = sinf(camPitch);
	float r = cosf(camPitch);
	float z = r*cosf(camYaw);
	float x = r*sinf(camPitch);

	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camTarget = XMVector3Normalize(camTarget);

	camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camUp = XMVector3Cross(camForward, camRight);

	camPosition += moveLeftRight*camRight;
	camPosition += moveBackForward*camForward;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	camTarget = camPosition + XMVectorSet(x, y, z, 0.0f);

	view = XMMatrixLookAtLH(camPosition, camTarget, camUp);

}

void Movement::detectKeys(int &currentScene)
{
	auto kb = mKeyboard->GetState();
	//float speed = 0.005f;
	if (kb.F1)
	{
		currentScene = Scenes::SceneOne;
	}
	if (kb.F2)
	{
		currentScene = Scenes::SceneTwo;
	}
	if (kb.F3)
	{
		currentScene = Scenes::SceneThree;
	}
	if (kb.W)
	{
		moveBackForward += MOVESPEED;
	}
	if (kb.S)
	{
		moveBackForward -= MOVESPEED;
	}
	if (kb.D)
	{
		moveLeftRight += MOVESPEED;
	}
	if (kb.A)
	{
		moveLeftRight -= MOVESPEED;
	}
	

	auto currState = mMouse->GetState();

	if (currState.positionMode == Mouse::MODE_RELATIVE)
	{
		XMFLOAT3 delta;
		
		delta.x = float(currState.x);
		delta.y = float(currState.y);
		delta.z = 0.0f;
		

		if (currState.x != startState.x || currState.y != startState.y)
		{
			camYaw -= float(startState.x) * 0.001f;
			camPitch -= float(startState.y) * 0.001f;

			startState = currState;
		}
		/*camPitch -= delta.y;
		camYaw -= delta.x;

		float limit = XM_PI / 2.0f - 0.01f;
		camPitch = max(-limit, camPitch);
		camPitch = min(+limit, camPitch);

		if (camYaw > XM_PI)
		{
			camYaw -= XM_PI * 2.0f;
		}
		else if (camYaw < -XM_PI)
		{
			camYaw += XM_PI * 2.0f;
		}*/
	}
	mMouse->SetMode(currState.leftButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

	

	return;
}
