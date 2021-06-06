#pragma once
#include "Game.h"
#include "CameraController.h"
#include "PlaneComponent.h"
#include "LightTexturedComponent.h"
class LightTextureGame : public Game
{
public:
	Camera* cam = nullptr;
	CameraController* camController = nullptr;	
	LightTextureGame(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) :Game(hInstance, hPrevInstance, pScmdline, iCmdshow)
	{
		tGame = this;
	}
	virtual void Initialize() override;
	virtual void Update(float deltaTime) override;
	virtual void PostDraw(float deltaTime) override;

	void OnMouseMove(InputDevice::MouseMoveEventArgs& args);
};