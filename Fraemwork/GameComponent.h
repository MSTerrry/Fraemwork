#pragma once
#include "Libs.h"
using namespace DirectX::SimpleMath;
class GameComponent {
public:
	GameComponent* Parent = nullptr;
	float curScale = 1;
	float curRotation;
	float scaleIteration = 1;
	float lastScale = 1;
	Matrix World;
	virtual ~GameComponent() = default;	
	GameComponent() {

	}
	virtual HRESULT Initialize() = 0;
	virtual void Reload() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Draw(float deltaTime) = 0;
	virtual void DestroyResources() = 0;	
};