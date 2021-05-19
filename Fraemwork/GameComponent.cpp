#include "GameComponent.h"

HRESULT GameComponent::DestroyResources() {
	return S_OK;
}
HRESULT GameComponent::Draw() {
	return S_OK;
}
GameComponent::GameComponent() {
	
}
HRESULT GameComponent::Initialize(ID3D11Device *device) {
	return S_OK;
}
HRESULT GameComponent::Reload() {
	return S_OK;
}
HRESULT GameComponent::Update() {
	return S_OK;
}