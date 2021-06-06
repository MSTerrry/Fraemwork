#pragma once
#include "Game.h"
#include "CameraController.h"
#include "PlaneComponent.h"
class TransformGame : public Game
{
public:
	Camera* cam = nullptr;
	CameraController* camController = nullptr;
	TransformGame(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) :Game(hInstance, hPrevInstance,pScmdline, iCmdshow)
	{
		tGame = this;
	}
	virtual void Initialize() override;
	virtual void Update(float deltaTime) override;
	virtual void PostDraw(float deltaTime) override;

	void OnMouseMove(InputDevice::MouseMoveEventArgs& args);
};