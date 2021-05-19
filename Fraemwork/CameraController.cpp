#include "CameraController.h"

CameraController::CameraController(InputDevice* inDev,Camera* camera):camera(camera), inputDevice(inDev){
	Yaw = 0;
	Pitch = 0;
	CameraController& cont = *this;

	inputDevice->MouseMove.AddRaw(this, &CameraController::OnMouseMove);
}

void CameraController::Update(float deltaTime) {
	auto rotMat = Matrix::CreateFromYawPitchRoll(Yaw, Pitch, 0);
	auto velDirection = Vector3::Zero;
	if (inputDevice->IsKeyDown(0x57)) velDirection += Vector3(1.0f, 0.0f, 0.0f);//W
	if (inputDevice->IsKeyDown(0x53)) velDirection += Vector3(-1.0f, 0.0f, 0.0f);//S
	if (inputDevice->IsKeyDown(0x41)) velDirection += Vector3(0.0f, 0.0f, -1.0f);//A
	if (inputDevice->IsKeyDown(0x44)) velDirection += Vector3(0.0f, 0.0f, 1.0f);//D

	if (inputDevice->IsKeyDown(VK_SPACE)) velDirection += Vector3(0.0f, 1.0f, 0.0f);
	if (inputDevice->IsKeyDown(0x43)) velDirection += Vector3(0.0f, -1.0f, 0.0f);//C

	velDirection.Normalize();

	auto velDir = rotMat.Forward() * velDirection.x + Vector3::Up * velDirection.y + rotMat.Right() * velDirection.z;
	if (velDir.Length() != 0) {
		velDir.Normalize();
	}

	camera->ViewMatrix = Matrix::CreateLookAt(CameraPosition, CameraPosition * rotMat.Forward(), rotMat.Up());
	camera->UpdateProjectionMatrix();
}

void CameraController::OnMouseMove(const InputDevice::MouseMoveEventArgs& args) {
	if (inputDevice->IsKeyDown(VK_LSHIFT)) return;

	Yaw -= args.Offset.x * 0.003f * MouseSensetivity;
	Pitch -= args.Offset.y * 0.003f * MouseSensetivity;

	if (args.WheelDelta == 0) return;
	if (args.WheelDelta > 0) VelocityMagnitude += 1;
	if (args.WheelDelta < 0) VelocityMagnitude -= 1;
}