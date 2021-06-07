#include "TrueTriangleComponent.h"
using namespace DirectX::SimpleMath;
void TrueTriangleComponent::DestroyResources() {
	if (layout) layout->Release();
	if (vertexShader) vertexShader->Release();
	if (pixelShader) pixelShader->Release();
}

TrueTriangleComponent::TrueTriangleComponent(ID3D11Device* device, ID3D11DeviceContext* context, Camera* camera, Vector3 pos, Vector4* points, bool zTranslation)
	:device(device), context(context), camera(camera),points(points),position(pos),zTranslation(zTranslation)
{	
	Initialize();
}

void TrueTriangleComponent::Draw(float deltaTime) {

	ID3D11RasterizerState* oldState;

	const UINT stride = 32;
	const UINT offset = 0;

	context->RSGetState(&oldState);
	context->RSSetState(rastState);

	context->IASetInputLayout(layout);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(0, 1, &vertices, &stride, &offset);
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	context->VSSetConstantBuffers(0, 1, &constantBuffer);

	annotation->BeginEvent(L"BeginDraw");
	context->Draw(3, 0);
	annotation->EndEvent();

	context->RSSetState(rastState);
	if (oldState)
		oldState->Release();

}
HRESULT TrueTriangleComponent::CreateShader(LPCWSTR fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** vertexBC, D3D_SHADER_MACRO* shaderMacros) {
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

HRESULT TrueTriangleComponent::Initialize() {
	HRESULT res;
	//MiniTri.fx
	res = CreateShader(L"Simple.hlsl", "VSMain", "vs_5_0", &VertexShaderByteCode, nullptr);
	if (FAILED(res)) return res;

	res = CreateShader(L"Simple.hlsl", "PSMain", "ps_5_0", &PixelShaderByteCode, nullptr);
	if (FAILED(res)) return res;

	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		D3D11_INPUT_ELEMENT_DESC { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	res = device->CreateVertexShader(VertexShaderByteCode->GetBufferPointer(), VertexShaderByteCode->GetBufferSize(), nullptr, &vertexShader);
	res = device->CreatePixelShader(PixelShaderByteCode->GetBufferPointer(), PixelShaderByteCode->GetBufferSize(), nullptr, &pixelShader);

	res = device->CreateInputLayout(inputElements, 2, VertexShaderByteCode->GetBufferPointer(), VertexShaderByteCode->GetBufferSize(), &layout);

	indeces = new int[]
	{
		0, 1, 2
	};
	D3D11_BUFFER_DESC indexBufDesc = {};
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	indexBufDesc.ByteWidth = sizeof(int) * 3;

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indeces;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	device->CreateBuffer(&indexBufDesc, &indexData, &indexBuffer);
	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = sizeof(Vector4) * 3*2;

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = points;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	res = device->CreateBuffer(&vertexBufDesc, &vertexData, &vertices);


	D3D11_BUFFER_DESC constBufDesc = {};
	constBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufDesc.MiscFlags = 0;
	constBufDesc.StructureByteStride = 0;
	constBufDesc.ByteWidth = sizeof(Matrix);
	res = device->CreateBuffer(&constBufDesc, nullptr, &constantBuffer);


	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_NONE
	rastDesc.FillMode = D3D11_FILL_SOLID;
	

	res = device->CreateRasterizerState(&rastDesc, &rastState);
	res = context->QueryInterface(IID_ID3DUserDefinedAnnotation, (void**)&annotation);

	return res;
}
void TrueTriangleComponent::Update(float deltaTime) {

	auto resVector = Vector3(position.x, position.y, position.z);
	if (zTranslation) {
		resVector.z = resVector.z * Parent->lastScale;
	}
	else resVector.x = resVector.x * Parent->lastScale;	
	
	auto rotationM = Matrix::CreateTranslation(-resVector) * (zTranslation? Matrix::CreateRotationZ(Parent->curRotation) : 
		Matrix::CreateRotationX(Parent->curRotation)) * Matrix::CreateTranslation(resVector);
	auto pos = rotationM * Matrix::CreateTranslation(resVector) * Parent->World;
	auto m = pos * camera->ViewMatrix * camera->ProjMatrix;
	D3D11_MAPPED_SUBRESOURCE res = {};
	context->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	auto dataP = reinterpret_cast<float*>(res.pData);
	memcpy(dataP, &m, sizeof(Matrix));
	context->Unmap(constantBuffer, 0);
}
void TrueTriangleComponent::Reload() {

}