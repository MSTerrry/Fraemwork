#include "CameraController.h"

CameraController::CameraController(InputDevice* inDev,Camera* camera):camera(camera), inputDevice(inDev)
{	
	Yaw = 0;
	Pitch = 0;
	CameraController& cont = *this;
	CameraPosition = DirectX::SimpleMath::Vector3(0, 1, 1);
	inputDevice->MouseMove.AddRaw(this, &CameraController::OnMouseMove);
}

void CameraController::Update(float deltaTime) {
	auto rotMat = Matrix::CreateFromYawPitchRoll(Yaw, Pitch, 0);
	auto velDirection = Vector3::Zero;
	if (inputDevice->IsKeyDown(Keys::W))
		velDirection += Vector3(1.0f, 0.0f, 0.0f);
	if (inputDevice->IsKeyDown(Keys::S)) velDirection += Vector3(-1.0f, 0.0f, 0.0f);
	if (inputDevice->IsKeyDown(Keys::A)) velDirection += Vector3(0.0f, 0.0f, -1.0f);
	if (inputDevice->IsKeyDown(Keys::D)) velDirection += Vector3(0.0f, 0.0f, 1.0f);

	if (inputDevice->IsKeyDown(Keys::Space)) velDirection += Vector3(0.0f, 1.0f, 0.0f);
	if (inputDevice->IsKeyDown(Keys::C)) velDirection += Vector3(0.0f, -1.0f, 0.0f);

	velDirection.Normalize();

	auto velDir = rotMat.Forward() * velDirection.x + Vector3::Up * velDirection.y + rotMat.Right() * velDirection.z;
	if (velDir.Length() != 0) {
		velDir.Normalize();
	}

	camera->ViewMatrix = Matrix::CreateLookAt(CameraPosition, CameraPosition * rotMat.Forward(), rotMat.Up());
	camera->UpdateProjectionMatrix();
}

void CameraController::OnMouseMove(const InputDevice::MouseMoveEventArgs& args) {
	if (inputDevice->IsKeyDown(Keys::LeftShift)) return;

	Yaw -= args.Offset.x * 0.003f * MouseSensetivity;
	Pitch -= args.Offset.y * 0.003f * MouseSensetivity;

	if (args.WheelDelta == 0) return;
	if (args.WheelDelta > 0) VelocityMagnitude += 1;
	if (args.WheelDelta < 0) VelocityMagnitude -= 1;
}