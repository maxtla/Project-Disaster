#pragma once
#ifndef MOVEMENT_H
#define MOVEMENT_H

#include<d3d11.h>
#include<DirectXMath.h>
//#include<dinput.h>
#include<time.h>
#include<d3dcompiler.h>
#include"DirectXHelpers.h"
#include"Mouse.h"
#include"Keyboard.h"
#include <chrono>

#define MOVESPEED 0.1f
#define MOVESPEED_HEIGHTMAP 1.0f
#define CAMYAWPITCHOFFSET 0.001f
#define CAMYAWPITCHOFFSET_HEIGHTMAP 0.01f
#define FRAME_UPDATES_MOVEMENT 60
#define MOVESPEED_SPHERE 0.05f

using namespace DirectX;
using namespace std;
using namespace chrono;

enum Scenes
{
	SceneOne,
	SceneTwo,
	SceneThree,
	SceneFour
};

class Movement
{
private:
	//cameraStuff
	XMVECTOR camPosition;
	XMVECTOR camTarget;
	XMVECTOR camUp;
	XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMMATRIX camRotationMatrix;
	float camYaw = 0.0f;
	float camPitch = 0.0f;
	//input variables
	unique_ptr<Keyboard> mKeyboard;
	unique_ptr<Mouse> mMouse;
	Mouse::State startState;
	high_resolution_clock::time_point start_clock_movement, current_clock_movement;
	high_resolution_clock::time_point rotation_clock_start, rotation_clock_current;

	
public:
	Movement();
	~Movement();
	void initialize(HWND hwnd);
	
	void updateCamera(XMMATRIX &view);
	void detectKeys(int &currentScene);
	void moveSphere(XMFLOAT3 &center);
	XMVECTOR getCamPos() const;
	XMVECTOR getTarget() const;
	XMVECTOR getTargetToCam() const;
};
#endif // !MOVEMENT_H

