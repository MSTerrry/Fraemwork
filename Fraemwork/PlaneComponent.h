#pragma once
#include "GameComponent.h"
using namespace DirectX;
class PlaneComponent : public GameComponent
{
	HRESULT CreateShader(LPCWSTR fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** vertexBC, D3D_SHADER_MACRO* shaderMacros);
	int FillLines(int startAmount, int endAmount, int indexInArray);
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3DBlob* PixelShaderByteCode;
	ID3DBlob* VertexShaderByteCode;
	ID3D11InputLayout* layout = nullptr;
	ID3D11Buffer* vertices = nullptr;
	ID3D11Buffer* constantBuffer = nullptr;
	ID3D11RasterizerState* rastState = nullptr;
	DirectX::SimpleMath::Vector4* points = nullptr;
	ID3DUserDefinedAnnotation* annotation = nullptr;
	int* indeces;
	ID3D11Device* device;	
	ID3D11DeviceContext* context;
	Camera* camera;
public:
	int amount;
	DirectX::SimpleMath::Vector3 position;
	PlaneComponent(ID3D11Device* device, ID3D11DeviceContext* context, Camera* camera);
	virtual void DestroyResources() override;
	virtual void Draw(float deltaTime) override;
	virtual HRESULT Initialize() override;
	virtual void Update(float deltaTime) override;
	virtual void Reload() override;

};