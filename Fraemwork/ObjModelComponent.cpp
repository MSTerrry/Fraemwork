#include "ObjModelComponent.h"
using namespace DirectX::SimpleMath;
struct ConstantData {
	Matrix WorldViewProj;
	Matrix World;
	Vector4 ClipPlane;
};

ObjModelComponent::ObjModelComponent(ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR inFileName, LPCWSTR inTextureName, Camera* camera):
	Cam(camera),Device(device),Context(context)
{
	texLoader = new TextureLoader(device, context);
}
HRESULT ObjModelComponent::CreateShader(LPCWSTR fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** vertexBC, D3D_SHADER_MACRO* shaderMacros) {
	HRESULT res;
	ID3DBlob* errorVertexCode;
	res = D3DCompileFromFile(fileName,
		shaderMacros /*macros*/,
		nullptr /*include*/,
		entryPoint,
		shaderModel,
		D3DCOMPILE_PACK_MATRIX_ROW_MAJOR,
		0,
		vertexBC,
		&errorVertexCode);

	if (FAILED(res)) {
		if (errorVertexCode) {
			char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());
			std::cout << compileErrors << std::endl;
		}
		else
			std::cout << "Missing Shader File " << fileName << std::endl;
	}
	if (errorVertexCode) errorVertexCode->Release();
	return res;
}
HRESULT ObjModelComponent::Initialize() {
	HRESULT res;
	res = CreateShader(L"ObjModelShader.hlsl", "VSMain", "vs_5_0", &VertexShaderByteCode, nullptr);
	if (FAILED(res)) return res;

	res = CreateShader(L"ObjModelShader.hlsl", "PSMain", "ps_5_0", &PixelShaderByteCode, nullptr);
	if (FAILED(res)) return res;

	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		D3D11_INPUT_ELEMENT_DESC { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		D3D11_INPUT_ELEMENT_DESC { "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		D3D11_INPUT_ELEMENT_DESC { "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		D3D11_INPUT_ELEMENT_DESC { "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		D3D11_INPUT_ELEMENT_DESC { "TEXTCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	res = Device->CreateVertexShader(VertexShaderByteCode->GetBufferPointer(), VertexShaderByteCode->GetBufferSize(), nullptr, &vertexShader);
	if (FAILED(res)) return res;
	res = Device->CreatePixelShader(PixelShaderByteCode->GetBufferPointer(), PixelShaderByteCode->GetBufferSize(), nullptr, &pixelShader);
	if (FAILED(res)) return res;

	res = Device->CreateInputLayout(inputElements, 6, VertexShaderByteCode->GetBufferPointer(), VertexShaderByteCode->GetBufferSize(),&layout);
	//objLoader->LoadObjModel(modelName, vertices, elemCount);
	

	D3D11_BUFFER_DESC constBufDesc = {};
	constBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufDesc.MiscFlags = 0;
	constBufDesc.StructureByteStride = 0;
	constBufDesc.ByteWidth = sizeof(ConstantData);
	res = Device->CreateBuffer(&constBufDesc, nullptr, &constantBuffer);


	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_NONE
	rastDesc.FillMode = D3D11_FILL_SOLID;

	res = Device->CreateRasterizerState(&rastDesc, &rastState);
	texLoader->LoadTextureFromFile(textureName, texture, texSrv, true, false);	

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MaxLOD = INT_MAX;
	res = Device->CreateSamplerState(&samplerDesc, &sampler);

	return res;
}

void ObjModelComponent::Reload() {

}
void ObjModelComponent::Update(float deltaTime) {
	auto data = ConstantData{};
	data.World = Transform * Matrix::CreateTranslation(Position);
	data.WorldViewProj = data.World * Cam->GetCameraMatrix();
	data.ClipPlane = ClipPlane;
	Context->UpdateSubresource(constantBuffer, 0, nullptr, &data, 0, 0);
}
void ObjModelComponent::Draw(float deltaTime) {
	ID3D11RasterizerState* oldState;
	const UINT stride = 96;
	const UINT offset = 0;

	Context->RSGetState(&oldState);
	Context->RSSetState(rastState);

	Context->IASetInputLayout(layout);
	Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context->IASetVertexBuffers(0, 1, &vertices, &stride, &offset);

	Context->VSSetShader(vertexShader, nullptr, 0);
	Context->PSSetShader(pixelShader, nullptr, 0);

	Context->VSSetConstantBuffers(0, 1, &constantBuffer);
	Context->PSSetShaderResources(0, 1, &texSrv);
	Context->PSSetSamplers(0, 1, &sampler);

	Context->Draw(elemCount, 0);
	Context->RSSetState(oldState);
	if (oldState)
		oldState->Release();

}
void ObjModelComponent::DestroyResources() {
	if (texSrv) texSrv->Release();
	if (rastState) rastState->Release();
	if (layout) layout->Release();
	if (vertexShader) vertexShader->Release();
	if (VertexShaderByteCode) VertexShaderByteCode->Release();
	if (pixelShader) pixelShader->Release();
	if (PixelShaderByteCode) PixelShaderByteCode->Release();
	if (vertices) vertices->Release();	
}