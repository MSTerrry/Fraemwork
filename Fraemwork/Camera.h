#pragma once
#include <d3d.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_4.h>
#include "DisplayWin32.h"
#include <DirectXMath.h>
#include "SimpleMath.h"
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