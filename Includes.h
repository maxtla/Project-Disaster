#include "Application.h"
#include "DeferredBuffer.h"
#include "DeferredShader.h"
#include "LightShader.h"
#include "Model.h"
#include "ModelLoader.h"
#include "NormalMapShader.h"
#include "SceneDeferredRendering.h"
#include "SceneInterface.h"
#include "SceneNormalMapping.h"
#include "WICTextureLoader.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <chrono>
#include <DirectXMath.h>

//collect comments for the linker to include libraries here
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

//collect namespace:s here
using namespace DirectX;
using namespace std;

//collect macros here
#define WIDTH 1280
#define HEIGHT 720