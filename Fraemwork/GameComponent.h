#pragma once
#include <d3d.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <iostream>
#include "Camera.h"
#include <directxmath.h>
#include <vector>
#include "SimpleMath.h"
#pragma comment(lib, "d3d11.lib")

class GameComponent {
public:
	//Game game;
	virtual ~GameComponent() = default;	
	GameComponent() {

	}
	virtual HRESULT Initialize() = 0;
	virtual void Reload() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Draw(float deltaTime) = 0;
	virtual void DestroyResources() = 0;	
};