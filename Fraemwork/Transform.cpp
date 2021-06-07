#include "Transform.h"

Transform::Transform(Vector3 pos, float scale):position(pos),scale(scale)
{
}

void Transform::Update() {
	world = CalculateMatrix();
}
Matrix Transform::CalculateMatrix() {
	auto result =  Matrix::CreateScale(scale) *
		Matrix::CreateFromQuaternion(rotation) *
		Matrix::CreateTranslation(position);
	if (Parent != nullptr)
		result = result * Parent->CalculateMatrix();
	return result;
}