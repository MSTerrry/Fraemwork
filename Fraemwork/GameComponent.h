#pragma once
#include "Libs.h"
#pragma comment(lib, "d3d11.lib")

class GameComponent {
public:
	virtual ~GameComponent() = default;	
	GameComponent() {

	}
	virtual HRESULT Initialize() = 0;
	virtual void Reload() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Draw(float deltaTime) = 0;
	virtual void DestroyResources() = 0;	
};