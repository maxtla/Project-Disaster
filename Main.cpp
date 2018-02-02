#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <string>
#include "ModelLoader.h"
#include "DeferredShader.h"
#include "DeferredBuffer.h"
#include "LightShader.h"
#include<dinput.h>


//collect comments for the linker to include libraries here
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib,"dinput8.lib")


//collect namespace:s here
using namespace DirectX;
using namespace std;

//collect macros here
#define WIDTH 1280
#define HEIGHT 720

//collect variables and globals here
LPCTSTR WndClassName = L"wnd";
HWND hwnd = NULL; //window handler
	//directX pointers
IDXGISwapChain* pSwapChain;
ID3D11Device* pDev;
ID3D11DeviceContext* pDevCon;
ID3D11RenderTargetView* pRTV;
ID3D11DepthStencilView* pDSV;
ID3D11Texture2D* pDSBuffer;
D3D11_VIEWPORT _viewPort;
ID3D11DepthStencilState* pDepthStencilState;
ID3D11DepthStencilState* pDepthDisabledStencilState;
ID3D11RasterizerState* pRasterState;
ID3D11RasterizerState* pRasterStateNoCulling;
//own classes
ModelLoader *pModelLoader;
DeferredBuffer* pDeferredBuffer;
DeferredShader* pDeferredShader;
LightShader* pLightShader;

//shoudl maybe create a light object class for this
XMVECTOR lightPos = XMVectorSet(0.0f, 0.0f, -5.0f, 1.f);
//maybe create a camera klass for this?
XMMATRIX view = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -3.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PI * 0.45f, ((float)WIDTH) / HEIGHT, 0.1f, 20.0f);
XMMATRIX world = XMMatrixIdentity();
//2D projection matrix
XMMATRIX _2D_projection = XMMatrixOrthographicLH((float)WIDTH, (float)HEIGHT, 0.1f, 1000.0f);
//erase this later
//---
float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;
int colormodr = 1;
int colormodg = 1;
int colormodb = 1;
float rotationValue = 0;
//---

//Camera stuff for movement
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


//input variables
IDirectInputDevice8* IDKeyboard;
IDirectInputDevice8* IDMouse;
DIMOUSESTATE mouseLastState;
LPDIRECTINPUT8 DirectInput;
float moveLeftRight = 0.0f;
float moveBackForward = 0.0f;

//time variables
double countsPerSec = 0.0;
_int64 counterStart = 0;
double fTime = 0.0;
_int64 fTimeOld = 0;
int frameCount = 0;
int fps = 0;


//collect function prototypes here
bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed);
int msgLoop();
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool initD3D11App(HINSTANCE hInstance);
bool initDirectInput(HINSTANCE hInstance);
void releaseObjects(); //avoid memory leaks
bool initScene();
void updateScene();
void updateCamera();
void renderScene();
void startTimer();
double getTime();
double getFrameTime();
void detectKeys(double time);

//Main entry point
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{
	//initialize window
	if (!InitializeWindow(hInstance, nShowCmd, WIDTH, HEIGHT, true))
	{
		//if failed, display error msg
		MessageBox(0, L"Window initialization failed", L"ERROR", MB_OK);
		return 0;
	}
	//initialize Direct3D
	if (!initD3D11App(hInstance))
	{
		MessageBox(0, L"Direct3D initialization failed", L"ERROR", MB_OK);
		return 0;
	}
	//inizialize scene
	if (!initScene())
	{
		MessageBox(0, L"Scene initialization failed", L"ERROR", MB_OK);
		return 0;
	}
	if (!initDirectInput(hInstance))
	{
		MessageBox(0, L"Direct3D initialization failed", L"ERROR", MB_OK);
		return 0;
	}
	msgLoop(); //the core of the program
	releaseObjects();
	return 0;
}


bool InitializeWindow
	(HINSTANCE hInstance,    //Initialize our window
	int ShowWnd,
	int width, 
	int height,
	bool windowed)
{
	//Start creating the window//

	WNDCLASSEX wc;    //Create a new extended windows class

	wc.cbSize = sizeof(WNDCLASSEX);    //Size of our windows class
	wc.style = CS_HREDRAW | CS_VREDRAW;    //class styles
	wc.lpfnWndProc = WndProc;    //Default windows procedure function
	wc.cbClsExtra = NULL;    //Extra bytes after our wc structure
	wc.cbWndExtra = NULL;    //Extra bytes after our windows instance
	wc.hInstance = hInstance;    //Instance to current application
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);    //Title bar Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);    //Default mouse Icon
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);    //Window bg color
	wc.lpszMenuName = NULL;    //Name of the menu attached to our window
	wc.lpszClassName = WndClassName;    //Name of our windows class
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO); //Icon in your taskbar

	if (!RegisterClassEx(&wc))    //Register our windows class
	{
		//if registration failed, display error
		MessageBox(NULL, L"Error registering class",
			L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	hwnd = CreateWindowEx(    //Create our Extended Window
		NULL,    //Extended style
		WndClassName,    //Name of our windows class
		L"Project Disaster",    //Name in the title bar of our window
		WS_OVERLAPPEDWINDOW,    //style of our window
		CW_USEDEFAULT, CW_USEDEFAULT,    //Top left corner of window
		width,    //Width of our window
		height,    //Height of our window
		NULL,    //Handle to parent window
		NULL,    //Handle to a Menu
		hInstance,    //Specifies instance of current program
		NULL    //used for an MDI client window
	);

	if (!hwnd)    //Make sure our window has been created
	{
		//If not, display error
		MessageBox(NULL, L"Error creating window",
			L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	ShowWindow(hwnd, ShowWnd);    //Shows our window
	UpdateWindow(hwnd);    //Its good to update our window

	return true;    //if there were no errors, return true
}

int msgLoop() {    //The message loop

	MSG msg;    //Create a new message structure
	ZeroMemory(&msg, sizeof(MSG));    //clear message structure to NULL

	while (true)    //while there is a message
	{
		//if there was a windows message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)    //if the message was WM_QUIT
				break;    //Exit the message loop

			TranslateMessage(&msg);    //Translate the message
			DispatchMessage(&msg);	   //Send the message to default windows procedure
			
		}
		else 
		{    
			frameCount++;
			if (getTime() > 1.0f)
			{
				fps = frameCount;
				frameCount = 0;
				startTimer();
			}
			fTime = getFrameTime();
			//run game code here
			detectKeys(fTime);
			updateScene();
			renderScene();
		}

	}

	return (int)msg.wParam;        //return the message

}

LRESULT CALLBACK WndProc(HWND hwnd,    //Default windows procedure
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (msg)    //Check message
	{

	case WM_KEYDOWN:    //For a key down
						//if escape key was pressed, display popup box
		if (wParam == VK_ESCAPE) {
			if (MessageBox(0, L"Are you sure you want to exit?",
				L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)

				//Release the windows allocated memory  
				DestroyWindow(hwnd);
		}
		return 0;

	case WM_DESTROY:    //if x button in top right was pressed
		PostQuitMessage(0);
		return 0;
	}
	//return the message for windows to handle it
	return DefWindowProc(hwnd,
		msg,
		wParam,
		lParam);
}

bool initD3D11App(HINSTANCE hInstance)
{
	HRESULT hr;
	//buffer description
	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = WIDTH;
	bufferDesc.Height = HEIGHT;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//swapchain description
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 4;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//create swapchain
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDev, NULL, &pDevCon);
	if (FAILED(hr))
	{
		exit(-1);
	}
	//create backbuffer
	ID3D11Texture2D* backBuffer;
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (FAILED(hr))
	{
		exit(-1);
	}
	//create RTV
	hr = pDev->CreateRenderTargetView(backBuffer, NULL, &pRTV);
	backBuffer->Release();
	if (FAILED(hr))
	{
		exit(-1);
	}

	//describe the DSBuffer (depth-stencil buffer)
	D3D11_TEXTURE2D_DESC dpDesc;
	ZeroMemory(&dpDesc, sizeof(D3D11_TEXTURE2D_DESC));

	dpDesc.Width = WIDTH;
	dpDesc.Height = HEIGHT;
	dpDesc.MipLevels = 1;
	dpDesc.ArraySize = 1;
	dpDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dpDesc.SampleDesc.Count = 4;
	dpDesc.SampleDesc.Quality = 0;
	dpDesc.Usage = D3D11_USAGE_DEFAULT;
	dpDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dpDesc.CPUAccessFlags = 0;
	dpDesc.MiscFlags = 0;

	//create depth stecil view
	pDev->CreateTexture2D(&dpDesc, NULL, &pDSBuffer);


	//set up depth stencil desc
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//create the normal depth stencil state
	hr = pDev->CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState);
	if (FAILED(hr))
		return false;
	//Set the depth stencil state
	pDevCon->OMSetDepthStencilState(pDepthStencilState, 1);
	//initialize depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	//prepare the depth stencil view desc
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	//create depth stencil view
	pDev->CreateDepthStencilView(pDSBuffer, &depthStencilViewDesc, &pDSV);
	//set render target and bind depth-stencil view object
	pDevCon->OMSetRenderTargets(1, &pRTV, pDSV); 
	//create and set the raster description and raster state
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = true;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	//create rasterizer state
	hr = pDev->CreateRasterizerState(&rasterDesc, &pRasterState);
	if (FAILED(hr))
		return false;
	//set rasterizer state
	pDevCon->RSSetState(pRasterState);
	// Setup a raster description which turns off back face culling.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	//create no culling rasterizer state
	hr = pDev->CreateRasterizerState(&rasterDesc, &pRasterStateNoCulling);
	if (FAILED(hr))
		return false;

	// Setup the viewport for rendering.
	_viewPort.Width = (float)WIDTH;
	_viewPort.Height = (float)HEIGHT;
	_viewPort.MinDepth = 0.0f;
	_viewPort.MaxDepth = 1.0f;
	_viewPort.TopLeftX = 0.0f;
	_viewPort.TopLeftY = 0.0f;

	// Create the viewport.
	pDevCon->RSSetViewports(1, &_viewPort);

	//create the second depth stencil state for 2D rendering (Z buffer disabled)
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = pDev->CreateDepthStencilState(&depthDisabledStencilDesc, &pDepthDisabledStencilState);
	if (FAILED(hr))
		return false;
	
	

	return true;
}

bool initDirectInput(HINSTANCE hInstance)
{
	HRESULT hr;
	hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL);
	if (FAILED(hr))
	{
		exit(-1);
	}
	hr = DirectInput->CreateDevice(GUID_SysKeyboard, &IDKeyboard, NULL);
	if (FAILED(hr))
	{
		exit(-1);
	}
	hr = DirectInput->CreateDevice(GUID_SysMouse, &IDMouse, NULL);
	if (FAILED(hr))
	{
		exit(-1);
	}
	hr = IDKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = IDKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
	{
		exit(-1);
	}
	hr = IDMouse->SetDataFormat(&c_dfDIMouse);
	hr = IDMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	if (FAILED(hr))
	{
		exit(-1);
	}
	return true;
}

void releaseObjects()
{
	pSwapChain->SetFullscreenState(false, NULL);
	PostMessage(hwnd, WM_DESTROY, 0, 0);
	pSwapChain->Release();
	pDev->Release();
	pDevCon->Release();
	pDSV->Release();
	pDSBuffer->Release();
	pDeferredBuffer->Release();
	pDeferredShader->Release();
	pDepthStencilState->Release();
	pDepthDisabledStencilState->Release();
	pRasterState->Release();
	pRasterStateNoCulling->Release();
	pModelLoader->Release();
	pLightShader->Release();
	
	IDKeyboard->Release();
	IDMouse->Release();
	DirectInput->Release();
}

bool initScene()
{
	//here we have to create objects, load in shaders, set viewport, load models, textures, sounds, etc.
	if (pModelLoader == nullptr)
		pModelLoader = new ModelLoader();
	//load in Cube.obj
	if (!pModelLoader->load(pDev, "Assets//monkey.obj"))
		return false;
	//init deferred buffer
	if (pDeferredBuffer == nullptr)
		pDeferredBuffer = new DeferredBuffer();

	if (!pDeferredBuffer->initialize(pDev, WIDTH, HEIGHT, 1, 0))
		return false;
	//init deferred shader
	if (pDeferredShader == nullptr)
		pDeferredShader = new DeferredShader();

	if (!pDeferredShader->initialize(pDev, hwnd))
		return false;
	//init light shader
	if (pLightShader == nullptr)
		pLightShader = new LightShader();
	
	if (!pLightShader->initialize(pDev, hwnd))
		return false;


	return true;
}
void startTimer()
{
	LARGE_INTEGER freq;
	QueryPerformanceCounter(&freq);
	countsPerSec = double(freq.QuadPart);
	QueryPerformanceCounter(&freq);
	counterStart = freq.QuadPart;
}
double getTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart - counterStart) / countsPerSec;
}
double getFrameTime()
{
	LARGE_INTEGER currentTime;
	_int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart - fTimeOld;
	fTimeOld = currentTime.QuadPart;

	if (tickCount < 0.0f)
		tickCount = 0.0f;
	return float(tickCount) / countsPerSec;
}
void updateCamera()
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
void updateScene()
{
	//here we update objects ect. 
	
	//world =  XMMatrixMultiply( XMMatrixRotationY(rotationValue), XMMatrixRotationX(rotationValue - 0.00002f));
	world = XMMatrixRotationY(rotationValue);
	rotationValue += 0.00005f;
}

void renderScene()
{
	// ------ FIRST PASS -----
	//first we must render the scene using the deferred shader
	pDeferredBuffer->setRenderTargets(pDevCon);
	pDeferredBuffer->clearRenderTargets(pDevCon, 0.0f, 0.0f, 0.0f, 1.0f);
	for (int i = 0; i < pModelLoader->size(); i++)
	{
		pModelLoader->getModel(i).Render(pDevCon);
		pDeferredShader->render(pDevCon, pModelLoader->getModel(i).getVertexCount(), world, view, projection, pModelLoader->getModel(i).getTexture(), pModelLoader->getModel(i).getSampler());
	}



	// ------ LIGHT PASS -----
	//reset the render target back to the original backbuffer
	pDevCon->OMSetRenderTargets(1, &pRTV, pDSV);
	//reset the viewport
	pDevCon->RSSetViewports(1, &_viewPort);
	//clear scene for 2D rendering
	float color[4] = { 0.f, 0.f, 0.f, 1.0f };
	pDevCon->ClearRenderTargetView(pRTV, color);
	pDevCon->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	//setup scene for 2D rendering - disable depth
	pDevCon->OMSetDepthStencilState(pDepthDisabledStencilState, 1);

	for (int i = 0; i < pModelLoader->size(); i++)
	{
		pLightShader->Render(pDevCon, world, view, projection, pDeferredBuffer->getShaderResourceView(0), 
							 pDeferredBuffer->getShaderResourceView(1), pDeferredBuffer->getShaderResourceView(2), 
							 pModelLoader->getModel(i).getMaterialBuffer(), lightPos);
	}
	
	//enable depth
	pDevCon->OMSetDepthStencilState(pDepthStencilState, 1);

	pSwapChain->Present(0, 0);
}

void detectKeys(double time)
{
	DIMOUSESTATE mouseCurrState;

	BYTE keyBoardState[256];

	IDKeyboard->Acquire();
	IDMouse->Acquire();

	IDMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
	IDKeyboard->GetDeviceState(sizeof(keyBoardState), (LPVOID)&keyBoardState);
	
	if (keyBoardState[DIK_ESCAPE] & 0x80)
		PostMessage(hwnd, WM_DESTROY, 0, 0);
	float speed = 15.0f * time;

	if (keyBoardState[DIK_LEFT] & 0x80)
		moveLeftRight -= speed;
	if (keyBoardState[DIK_RIGHT] & 0x80)
		moveLeftRight += speed;
	if (keyBoardState[DIK_UP] & 0x80)
		moveBackForward += speed;
	if (keyBoardState[DIK_DOWN] & 0x80)
		moveBackForward -= speed;

	if (mouseCurrState.lX != mouseLastState.lX || mouseCurrState.lY != mouseLastState.lY)
	{
		camYaw += mouseLastState.lX * 0.001f;
		camPitch += mouseLastState.lY * 0.001f;

		mouseLastState = mouseCurrState;
	}
	updateCamera();

	return;
}