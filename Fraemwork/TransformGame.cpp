#include "TransformGame.h"

void TransformGame::Initialize() {
	cam = new Camera(Display);
	InDevice = new InputDevice(Display);
	camController = new CameraController(InDevice, cam);
	pc = new PlaneComponent(Device, Context, cam);
	pc->Initialize();	
	tri = new TriangleComponent(Device, Context, cam);
	tri->Initialize();
	Components.push_back(pc);
	Components.push_back(tri);
}
void TransformGame::Update(float deltaTime) {
	camController->Update(deltaTime);
	
	if (InDevice->IsKeyDown(Keys::Escape))
		Exit();
	Game::Update(deltaTime);
}
void TransformGame::PostDraw(float deltaTime) {

}

void TransformGame::OnMouseMove(InputDevice::MouseMoveEventArgs& args) {

}