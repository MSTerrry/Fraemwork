#pragma once
#include "Libs.h"
#include "GameComponent.h"
class CubeComponent : public GameComponent
{
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3DBlob* PixelShaderByteCode;
	ID3DBlob* VertexShaderByteCode;
	ID3D11InputLayout* layout = nullptr;
	ID3D11Buffer* vertices = nullptr;
	ID3D11Buffer* constantBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	ID3D11RasterizerState* rastState = nullptr;
	DirectX::SimpleMath::Vector4* points = nullptr;
	ID3DUserDefinedAnnotation* annotation = nullptr;
	int* indeces;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	Camera* camera;
	double curScale = 1;
	double curRotation;
	double scaleIteration = 1;
	double curCycle;
	
	HRESULT CreateShader(LPCWSTR fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** vertexBC, D3D_SHADER_MACRO* shaderMacros);
public:

	CubeComponent(ID3D11Device* device, ID3D11DeviceContext* context, Camera* camera);
	virtual void DestroyResources() override;
	virtual void Draw(float deltaTime) override;
	virtual HRESULT Initialize() override;
	virtual void Update(float deltaTime) override;
	virtual void Reload() override;

	DirectX::SimpleMath::Vector3 position;
};