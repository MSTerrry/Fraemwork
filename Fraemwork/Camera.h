#pragma once
#include "Libs.h"
#include "DisplayWin32.h"
class Camera {
	DisplayWin32* win=nullptr;
public:
	DirectX::SimpleMath::Matrix ViewMatrix;
	DirectX::SimpleMath::Matrix ProjMatrix;
	Camera(DisplayWin32* win);
	~Camera();
	DirectX::SimpleMath::Matrix GetCameraMatrix() const;
	DirectX::SimpleMath::Vector3 GetPosition()const;
	DirectX::SimpleMath::Vector3 GetForwardVector() const;

	void UpdateProjectionMatrix();
};