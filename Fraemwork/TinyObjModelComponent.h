#pragma once
#include "GameComponent.h"
#include "TextureLoader.h"
#include "tiny_obj_loader.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "SimpleMath.h"
#include "tiny_obj_loader.h"
#include <vector>
class TinyObjModelComponent:public GameComponent
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
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;	

	ID3D11ShaderResourceView* vSrv = nullptr;
	ID3D11ShaderResourceView* nSrv = nullptr;
	ID3D11ShaderResourceView* tSrv = nullptr;
	ID3D11ShaderResourceView* strSrv = nullptr;
	
	ID3D11BlendState* blendState = nullptr;
	ID3D11SamplerState* sampler = nullptr;
	
	UINT elemCount;
	ID3D11Buffer* vBuf = nullptr;
	ID3D11Buffer* nBuf = nullptr;
	ID3D11Buffer* tBuf = nullptr;
	ID3D11Buffer* strBuf = nullptr;
	std::vector<tinyobj::material_t> Materials;
	std::vector<tinyobj::shape_t> Shapes;
	DirectX::SimpleMath::Matrix Transform;

	TextureLoader* texLoader = nullptr;

	const char* modelName;

	Camera* camera = nullptr;
	HRESULT CreateShader(LPCWSTR fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** vertexBC, D3D_SHADER_MACRO* shaderMacros);
public:
	DirectX::SimpleMath::Vector3 Position;
	TinyObjModelComponent(ID3D11Device* device, ID3D11DeviceContext* context, Camera* camera, const char* fileName);
	virtual HRESULT Initialize() ;
	virtual void Reload();
	virtual void Update(float deltaTime);
	virtual void Draw(float deltaTime);
	virtual void DestroyResources();
};