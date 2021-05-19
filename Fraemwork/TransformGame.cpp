#include "TransformGame.h"

void TransformGame::Initialize() {
	cam = new Camera(Display);
	InDevice = new InputDevice(Display);
	camController = new CameraController(InDevice, cam);

	tri = new TriangleComponent(Device, Context, cam);

	Components.push_back(tri);	
}
void TransformGame::Update(float deltaTime) {
	camController->Update(deltaTime);
	float velocity = 500.0f;

	if (InDevice->IsKeyDown(VK_LEFT))
		tri->position += velocity * DirectX::SimpleMath::Vector3::Left * deltaTime;
	if (InDevice->IsKeyDown(VK_RIGHT))
		tri->position += velocity * DirectX::SimpleMath::Vector3::Right * deltaTime;
	if (InDevice->IsKeyDown(VK_LEFT))
		tri->position += velocity * DirectX::SimpleMath::Vector3::Up * deltaTime;
	if (InDevice->IsKeyDown(VK_LEFT))
		tri->position += velocity * DirectX::SimpleMath::Vector3::Down * deltaTime;

	if (InDevice->IsKeyDown(VK_ESCAPE))
		Exit();
	Game::Update(deltaTime);
}
void TransformGame::PostDraw(float deltaTime) {

}

void TransformGame::OnMouseMove(InputDevice::MouseMoveEventArgs& args) {

}