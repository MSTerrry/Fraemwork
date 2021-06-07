#pragma once
#include "Game.h"
#include "CameraController.h"
#include "PlaneComponent.h"
#include "CubeComponent.h"
class CubeWithTrianglesGame : public Game
{
public:
	Camera* cam = nullptr;
	CameraController* camController = nullptr;
	CubeWithTrianglesGame(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) :Game(hInstance, hPrevInstance, pScmdline, iCmdshow)
	{
		tGame = this;
	}
	virtual void Initialize() override;
	virtual void Update(float deltaTime) override;
	virtual void PostDraw(float deltaTime) override;
	void CreateTriangles(CubeComponent* cube);
	void OnMouseMove(InputDevice::MouseMoveEventArgs& args);
};