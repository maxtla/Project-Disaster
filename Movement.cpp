#include "Movement.h"



Movement::Movement()
{
	camPosition = XMVectorSet(0.0f,0.0f,0.0f,0.0f);
	camTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	camUp = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	view = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -3.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
}

Movement::~Movement()
{
}

void Movement::initialize()
{
	mKeyboard = make_unique<Keyboard>();
	mMouse = make_unique<Mouse>();
}

XMMATRIX Movement::getView()
{
	return view;
}

void Movement::updateCamera()
{
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

	camTarget = camPosition + camTarget;

	view = XMMatrixLookAtLH(camPosition, camTarget, camUp);
}

void Movement::detectKeys(float time)
{
	//DIMOUSESTATE mouseCurrState;
	
	//BYTE keyBoardState[256];
	//unique_ptr<Mouse> mMouseLast;
	

	/*if (keyBoardState[DIK_ESCAPE] & 0x80)
		PostMessage(hwnd, WM_DESTROY, 0, 0);
	float speed = 5.0f * time;

	if (keyBoardState[DIK_LEFT] & 0x80)
		moveLeftRight -= speed;
	if (keyBoardState[DIK_RIGHT] & 0x80)
		moveLeftRight += speed;
	if (keyBoardState[DIK_UP] & 0x80)
		moveBackForward += speed;
	if (keyBoardState[DIK_DOWN] & 0x80)
		moveBackForward -= speed;

	if (mMouse->GetState().x != mMouseLast->GetState().x || mouseCurrState.lY != mouseLastState.lY)
	{
		camYaw += mMouseLast * 0.001f;
		camPitch += mouseLastState.lY * 0.001f;

		mouseLastState = mouseCurrState;
	}*/
	auto kb = mKeyboard->GetState();

	if (kb.Home)
	{
		view = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -3.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
		camYaw = camPitch = 0;
	}
	//XMVECTOR move = XMVectorSet(0.0f, 0.0f, 0.0f,0.0f);

	float speed = 5.0f * time;
	if (kb.Up)
	{
		moveBackForward += speed;
	}
	if (kb.Down)
	{
		moveBackForward -= speed;
	}
	if (kb.Right)
	{
		moveLeftRight -= speed;
	}
	if (kb.Left)
	{
		moveLeftRight += speed;
	}
	updateCamera();

	return;
}
