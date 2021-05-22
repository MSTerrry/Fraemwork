#include "TransformGame.h"

void TransformGame::Initialize() {
	cam = new Camera(Display);
	InDevice = new InputDevice(Display);
	camController = new CameraController(InDevice, cam);

	tri = new TriangleComponent(Device, Context, cam);
	tri->Initialize();
	Components.push_back(tri);
}
void TransformGame::Update(float deltaTime) {
	camController->Update(deltaTime);
	float velocity = 500.0f;

	if (InDevice->IsKeyDown(Keys::Left))
		tri->position += velocity * DirectX::SimpleMath::Vector3::Left * deltaTime;
	if (InDevice->IsKeyDown(Keys::Right))
		tri->position += velocity * DirectX::SimpleMath::Vector3::Right * deltaTime;
	if (InDevice->IsKeyDown(Keys::Up))
		tri->position += velocity * DirectX::SimpleMath::Vector3::Up * deltaTime;
	if (InDevice->IsKeyDown(Keys::Down))
		tri->position += velocity * DirectX::SimpleMath::Vector3::Down * deltaTime;

	if (InDevice->IsKeyDown(Keys::Escape))
		Exit();
	Game::Update(deltaTime);
}
void TransformGame::PostDraw(float deltaTime) {

}

void TransformGame::OnMouseMove(InputDevice::MouseMoveEventArgs& args) {

}