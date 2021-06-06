#include "LightTextureGame.h"


void LightTextureGame::Initialize() {
	cam = new Camera(Display);	
	InDevice = new InputDevice(Display);
	camController = new CameraController(InDevice, cam);
	auto pc = new PlaneComponent(Device, Context, cam);
	Components.push_back(pc);
	
	auto tri = new TriangleComponent(Device, Context, cam);
	Components.push_back(tri);

	auto tex = new LightTexturedComponent(Device,Context,cam,L"",L"Wall.png");
	Components.push_back(tex);	
}


void LightTextureGame::Update(float deltaTime) {
	camController->Update(deltaTime);
	if (InDevice->IsKeyDown(Keys::Escape))
		Exit();
	Game::Update(deltaTime);
}

void LightTextureGame::PostDraw(float deltaTime) {

}

void LightTextureGame::OnMouseMove(InputDevice::MouseMoveEventArgs& args) {

}