#pragma once
#include "Libs.h"
#include "TextureLoader.h"
#include "GameComponent.h"
using namespace DirectX::SimpleMath;
class LightTexturedComponent :public GameComponent
{
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3DBlob* PixelShaderByteCode;
	ID3DBlob* VertexShaderByteCode;
	ID3D11InputLayout* layout = nullptr;
	ID3D11Buffer* vertices = nullptr;
	ID3D11Buffer* constantBuffer = nullptr;
	ID3D11Buffer* lightBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	ID3D11RasterizerState* rastState = nullptr;
	DirectX::SimpleMath::Vector4* points = nullptr;
	ID3DUserDefinedAnnotation* annotation = nullptr;

	LPCWSTR objFileName;								
	LPCWSTR textureName;								
	ID3D11Texture2D* texture;							
	ID3D11ShaderResourceView* texSrv = nullptr;
	ID3D11SamplerState* sampler = nullptr;
	TextureLoader* texLoader;

	double x=0;
	double y=0;
	double curAngle=0;
	Matrix Transform;
	int* indeces;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	Camera* camera;
	HRESULT CreateShader(LPCWSTR fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** vertexBC, D3D_SHADER_MACRO* shaderMacros);
public:
	LightTexturedComponent(ID3D11Device* device, ID3D11DeviceContext* context, Camera* camera, LPCWSTR inFileName, LPCWSTR inTexName);
	virtual void DestroyResources() override;
	virtual void Draw(float deltaTime) override;
	virtual HRESULT Initialize() override;
	virtual void Update(float deltaTime) override;
	virtual void Reload() override;

	DirectX::SimpleMath::Vector3 position;
};