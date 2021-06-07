#pragma once
#include "Libs.h"
using namespace DirectX::SimpleMath;
class Transform {
	
public:
	Matrix world = Matrix::Identity;
	Vector3 position;
	float scale;
	Quaternion rotation;
	Transform* Parent = nullptr;
	Transform(Vector3 pos, float scale);
	void Update();
	Matrix CalculateMatrix();
};