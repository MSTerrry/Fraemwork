#pragma once
#include "windows.h"
#pragma warning(disable : 4267)

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include <iostream>
#include "winuser.h"
#include <wrl.h>
#include "DisplayWin32.h"
#include "TriangleComponent.h"
#include "GameComponent.h"
#include <vector>
#include "Game.h"
#include "InputDevice.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include <d3d.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <DirectXColors.h>
#include <chrono>
#include <thread>
#define ZCHECK(exp) if(FAILED(exp)) { printf("Check failed at file: %s at line %i", __FILE__, __LINE__); return 0; }

class Game {
public:	

	std::vector<GameComponent*> Components;
	DisplayWin32* Display;
	HINSTANCE Instance;
	InputDevice* InDevice;

	ID3D11Device* Device = nullptr;
	ID3D11Buffer* _vertexBuffer = nullptr;
	ID3D11Buffer* _indexBuffer = nullptr;
	ID3D11Buffer* _constantBuffer = nullptr;
	ID3D11Debug* _debug = nullptr;

	ID3D11DeviceContext* Context = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	IDXGISwapChain1* swapChain1 = nullptr;
	ID3D11RenderTargetView* RenderView = nullptr;
	ID3DUserDefinedAnnotation* DebugAnnotation = nullptr;

	std::chrono::time_point<std::chrono::steady_clock> PrevTime;
	float TotalTime = 0;
	unsigned int frameCount = 0;

	HRESULT PrepareResources();
	HRESULT InitShaders();
	
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
	int Draw(HWND hwnd);
	void DestroyResources();
	virtual void Initialize();
	void CreateBackBuffer(TriangleComponent tComp);
	virtual void Update(float deltaTime);
	virtual void PostDraw(float deltaTime);
	virtual void Exit();
	Game(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow);
};