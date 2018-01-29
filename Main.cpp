#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <string>
#include "ModelLoader.h"

//collect comments for the linker to include libraries here
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")


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
ModelLoader *pModelLoader;
//erase this later
//---
float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;
int colormodr = 1;
int colormodg = 1;
int colormodb = 1;
//---

//collect function prototypes here
bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed);
int msgLoop();
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool initD3D11App(HINSTANCE hInstance);
void releaseObjects(); //avoid memory leaks
bool initScene();
void updateScene();
void renderScene();


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
			//run game code here
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
	swapChainDesc.SampleDesc.Count = 1;
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
	dpDesc.SampleDesc.Count = 1;
	dpDesc.SampleDesc.Quality = 0;
	dpDesc.Usage = D3D11_USAGE_DEFAULT;
	dpDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dpDesc.CPUAccessFlags = 0;
	dpDesc.MiscFlags = 0;

	//create depth stecil view
	pDev->CreateTexture2D(&dpDesc, NULL, &pDSBuffer);
	pDev->CreateDepthStencilView(pDSBuffer, NULL, &pDSV);

	//set render target and bind depth-stencil view object
	pDevCon->OMSetRenderTargets(1, &pRTV, pDSV); //we might need to add a depth stencil view later

	return true;
}

void releaseObjects()
{
	pSwapChain->Release();
	pDev->Release();
	pDevCon->Release();
	pDSV->Release();
	pDSBuffer->Release();
}

bool initScene()
{
	//here we have to create objects, load in shaders, set viewport, load models, textures, sounds, etc.
	if (pModelLoader == nullptr)
		pModelLoader = new ModelLoader();
	//load in Cube.obj
	if (!pModelLoader->load(pDev, "Assets//Cube.obj"))
		return false;

	return true;
}

void updateScene()
{
	//here we update objects ect. 
	//for now we can just change the colors of the scene to show that it works
	red += colormodr * 0.00005f;
	green += colormodg * 0.00002f;
	blue += colormodb * 0.00001f;

	if (red >= 1.0f || red <= 0.0f)
		colormodr *= -1;
	if (green >= 1.0f || green <= 0.0f)
		colormodg *= -1;
	if (blue >= 1.0f || blue <= 0.0f)
		colormodb *= -1;
}

void renderScene()
{
	//clear out backbuffer and refresh the depth/stencul view
	float color[4] = { red, green, blue, 1.0f };
	pDevCon->ClearRenderTargetView(pRTV, color);
	pDevCon->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//handle drawing for models

	//present the backbuffer to the screen
	pSwapChain->Present(0, 0);
}