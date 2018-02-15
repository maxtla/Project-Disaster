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
#include<string>

#define MOVESPEED 0.005f

using namespace DirectX;
using namespace std;

enum Scenes
{
	SceneOne,
	SceneTwo,
	SceneThree,
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
	float camYaw = 0.0f;
	float camPitch = 0.0f;
	XMMATRIX camRotationMatrix;
	float current = 0.0f;
	Mouse::State startState;
	//input variables
	unique_ptr<Keyboard> mKeyboard;
	unique_ptr<Mouse> mMouse;
	//DIMOUSESTATE mouseLastState;
	
	float moveLeftRight = 0.0f;
	float moveBackForward = 0.0f;

	
public:
	Movement();
	~Movement();
	//bool initDirectInput(HINSTANCE hInstance, HWND hwnd);
	void initialize(HWND hwnd);
	
	void updateCamera(XMMATRIX &view);
	void detectKeys(int &currentScene);
};
#endif // !MOVEMENT_H
