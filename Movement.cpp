#include "Movement.h"



Movement::Movement()
{
	camPosition = XMVectorSet(0.0f,2.0f,-1.0f,0.0f);
	camTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
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
	this->start_clock_movement = high_resolution_clock::now();
	this->rotation_clock_start = high_resolution_clock::now();
}

void Movement::updateCamera(XMMATRIX &view)
{

	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camTarget = XMVector3Normalize(camTarget);

	camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camUp = XMVector3Cross(camForward, camRight);

	camTarget = camPosition + camTarget;

	view = XMMatrixLookAtLH(camPosition, camTarget, camUp);

}

void Movement::detectKeys(int &currentScene)
{
	auto kb = mKeyboard->GetState();
	XMVECTOR posToTarget = camTarget - camPosition;
	posToTarget = XMVector4Normalize(posToTarget);
	XMVECTOR sideVector = XMVector4Normalize(XMVector3Cross(posToTarget, camUp));

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
	if (kb.F4)
	{
		currentScene = Scenes::SceneFour;
	}

	//movement keys
	current_clock_movement = high_resolution_clock::now();
	duration<double, std::milli> delta_time = this->current_clock_movement - this->start_clock_movement;
	if (delta_time.count() > (1000 / FRAME_UPDATES_MOVEMENT))
	{
		
		if (kb.W)
		{
			if (currentScene == Scenes::SceneFour)
			{
				camPosition += posToTarget * MOVESPEED_HEIGHTMAP;
			}
			else
			{
				camPosition += posToTarget * MOVESPEED;
			}
		}
		if (kb.S)
		{
			if (currentScene == Scenes::SceneFour)
			{
				camPosition -= posToTarget * MOVESPEED_HEIGHTMAP;
			}
			else
			{
				camPosition -= posToTarget * MOVESPEED;
			}
		}
		if (kb.D)
		{
			if (currentScene == Scenes::SceneFour)
			{
				camPosition -= sideVector * MOVESPEED_HEIGHTMAP;
			}
			else
			{
				camPosition -= sideVector * MOVESPEED;
			}
		}
		if (kb.A)
		{
			if (currentScene == Scenes::SceneFour)
			{
				camPosition += sideVector * MOVESPEED_HEIGHTMAP;
			}
			else
			{
				camPosition += sideVector * MOVESPEED;
			}
		}
		this->start_clock_movement = high_resolution_clock::now();
	}
	

	auto currState = mMouse->GetState();

	rotation_clock_current = high_resolution_clock::now();
	delta_time = rotation_clock_current - rotation_clock_start;

	if (delta_time.count() > (1000 / FRAME_UPDATES_MOVEMENT))
	{
		if (currState.positionMode == Mouse::MODE_RELATIVE)
		{
			if (currState.x != startState.x || currState.y != startState.y)
			{
				camYaw += float(startState.x) * CAMYAWPITCHOFFSET;
				camPitch += float(startState.y) * CAMYAWPITCHOFFSET;
				startState = currState;
			}
		}
	}
	mMouse->SetMode(currState.leftButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

	

	return;
}

XMVECTOR Movement::getCamPos() const
{
	return this->camPosition;
}

XMVECTOR Movement::getTarget() const
{
	return this->camTarget;
}

XMVECTOR Movement::getTargetToCam() const
{
	return camPosition - camTarget;
}
