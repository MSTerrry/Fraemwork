#include "PlaneComponent.h"
using namespace DirectX::SimpleMath;

PlaneComponent::PlaneComponent(ID3D11Device* device, ID3D11DeviceContext* context, Camera* camera) : device(device), context(context), camera(camera)
{
	position = Vector3::Zero;
	amount = 200;
	points = new Vector4[amount];
}


void PlaneComponent::Draw(float deltaTime) {
	ID3D11RasterizerState* oldState;

	const UINT stride = 32;
	const UINT offset = 0;

	context->RSGetState(&oldState);
	context->RSSetState(rastState);

	context->IASetInputLayout(layout);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	context->IASetVertexBuffers(0, 1, &vertices, &stride, &offset);
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	context->VSSetConstantBuffers(0, 1, &constantBuffer);


	annotation->BeginEvent(L"BeginDraw");
	context->Draw(amount, 0);
	annotation->EndEvent();

	context->RSSetState(rastState);
	if (oldState)
		oldState->Release();
}

void PlaneComponent::Update(float deltaTime) {
	auto m = Matrix::CreateTranslation(position) * camera->ViewMatrix * camera->ProjMatrix;
	
	D3D11_MAPPED_SUBRESOURCE res = {};
	context->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	auto dataP = reinterpret_cast<float*>(res.pData);
	memcpy(dataP, &m, sizeof(Matrix));
	context->Unmap(constantBuffer, 0);
}
int PlaneComponent::FillLines(int startAmount, int endAmount, int indexInArray) {
	float x = -50.0f;
	float z = -18.75f;
	bool horizontalLines = indexInArray == 0;
	for (size_t i = startAmount; i < endAmount+ amount / 4; i++, indexInArray += 2)
	{
		if (i > startAmount && i % 2 == 0)
		{
			x = -50.0f;
			z += 1.5f;
		}
		points[indexInArray] = horizontalLines ?Vector4(x, 0.0, z, 1.0f) : Vector4(z, 0.0, x, 1.0f);
		points[indexInArray + 1] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		x = 50.0f;
	}
	return indexInArray;
}

HRESULT PlaneComponent::Initialize() {
	HRESULT res;
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
	
	auto k = 0;
	k = FillLines(0, 0, 0);
	FillLines(amount / 2, amount / 2, k);
	
	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = sizeof(Vector4) * amount;

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
HRESULT PlaneComponent::CreateShader(LPCWSTR fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** vertexBC, D3D_SHADER_MACRO* shaderMacros) {
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

void PlaneComponent::DestroyResources() {
	if (layout) layout->Release();
	if (vertexShader) vertexShader->Release();
	if (pixelShader) pixelShader->Release();
}
void PlaneComponent::Reload() {

}