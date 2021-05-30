#pragma once
#include "TextureLoader.h"
#include "Camera.h"
#include <DirectXMath.h>
#include "SimpleMath.h"
#include<d3dcompiler.h>
#include "GameComponent.h"
#include "ObjLoader.h"

class ObjModelComponent:public GameComponent {
	Camera* Cam = nullptr;
	LPCWSTR fileName;
	LPCWSTR textureName;
	ID3D11Device* Device=nullptr;
	ID3D11DeviceContext* Context = nullptr;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3DBlob* PixelShaderByteCode;
	ID3DBlob* VertexShaderByteCode;
	ID3D11InputLayout* layout = nullptr;
	ID3D11Buffer* vertices = nullptr;
	ID3D11Buffer* constantBuffer = nullptr;
	ID3D11RasterizerState* rastState = nullptr;
	ID3D11ShaderResourceView* texSrv = nullptr;
	ID3D11Texture2D* texture;
	DirectX::SimpleMath::Vector3 Position;
	DirectX::SimpleMath::Matrix Transform;
	DirectX::SimpleMath::Vector4 ClipPlane;

	TextureLoader* texLoader = nullptr;
	ObjLoader* objLoader = nullptr;
	ID3D11SamplerState* sampler = nullptr;
	int elemCount;

	HRESULT CreateShader(LPCWSTR fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** vertexBC, D3D_SHADER_MACRO* shaderMacros);
public:
	ObjModelComponent(ID3D11Device* Device, ID3D11DeviceContext* Context, LPCWSTR inFileName,LPCWSTR inTextureName, Camera* camera);
	virtual HRESULT Initialize();
	virtual void Reload();
	virtual void Update(float deltaTime);
	virtual void Draw(float deltaTime);
	virtual void DestroyResources();

};