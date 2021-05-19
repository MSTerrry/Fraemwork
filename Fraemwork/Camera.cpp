#include "Camera.h"
#define M_PI 3.14159265358979323846
Camera::Camera(DisplayWin32* win):win(win) {

	ViewMatrix = DirectX::SimpleMath::Matrix::Identity;
	UpdateProjectionMatrix();
}

Camera::~Camera() {

}

DirectX::SimpleMath::Matrix Camera::GetCameraMatrix() const {
	return ViewMatrix * ProjMatrix;
}

DirectX::SimpleMath::Vector3 Camera::GetPosition()const {
	DirectX::SimpleMath::Matrix inv;
	ViewMatrix.Invert(inv);
	return inv.Translation();
}
DirectX::SimpleMath::Vector3 Camera::GetForwardVector()const {

	DirectX::SimpleMath::Matrix inv;
	ViewMatrix.Invert(inv);
	return inv.Forward();
}

void Camera::UpdateProjectionMatrix() {
	ProjMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		(float)M_PI/2.0f,
		(float)win->ScreenWidth/win->ScreenHeight,
		0.1f,
		10000.0
	);
}
#undef M_PI