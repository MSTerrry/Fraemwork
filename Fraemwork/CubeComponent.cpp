#include "CubeComponent.h"

using namespace DirectX::SimpleMath;
void CubeComponent::DestroyResources() {
	if (layout) layout->Release();
	if (vertexShader) vertexShader->Release();
	if (pixelShader) pixelShader->Release();
}

CubeComponent::CubeComponent(ID3D11Device* device, ID3D11DeviceContext* context, Camera* camera) :device(device), context(context), camera(camera)
{
	position = Vector3(0, 0, 0);
	Initialize();
}

void CubeComponent::Draw(float deltaTime) {

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
	context->DrawIndexed(36, 0, 0);
	annotation->EndEvent();

	context->RSSetState(rastState);
	if (oldState)
		oldState->Release();

}
HRESULT CubeComponent::CreateShader(LPCWSTR fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** vertexBC, D3D_SHADER_MACRO* shaderMacros) {
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

HRESULT CubeComponent::Initialize() {
	HRESULT res;
	//MiniTri.fx
	res = CreateShader(L"Simple.hlsl", "VSMain", "vs_5_0", &VertexShaderByteCode, nullptr);
	if (FAILED(res)) return res;

	//D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };
	;
	res = CreateShader(L"Simple.hlsl", "PSMain", "ps_5_0", &PixelShaderByteCode, nullptr);
	if (FAILED(res)) return res;

	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		D3D11_INPUT_ELEMENT_DESC { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	res = device->CreateVertexShader(VertexShaderByteCode->GetBufferPointer(), VertexShaderByteCode->GetBufferSize(), nullptr, &vertexShader);
	res = device->CreatePixelShader(PixelShaderByteCode->GetBufferPointer(), PixelShaderByteCode->GetBufferSize(), nullptr, &pixelShader);

	res = device->CreateInputLayout(inputElements, 2, VertexShaderByteCode->GetBufferPointer(), VertexShaderByteCode->GetBufferSize(), &layout);

	points = new Vector4[]{
		 Vector4(1, 1, 1, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f), //позиция (от -1 до 1) //цвет
		Vector4(-1, -1, 1, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f),
		Vector4(1, -1, 1, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f),
		Vector4(-1, 1, 1, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4(1, 1, -1, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f), //позиция (от -1 до 1) //цвет
		Vector4(1, -1, -1, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f),
		Vector4(-1, 1, -1, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f),
		Vector4(-1, -1, -1, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)
	};
	indeces = new int[]
	{
		0, 1, 2,
		1, 0, 3,
		4, 2, 5,
		2, 4, 0,
		6, 5, 7,
		5, 6, 4,
		3, 7, 1,
		7, 3, 6,
		4, 3, 0,
		3, 4, 6,
		2, 7, 5,
		7, 2, 1
	};
	D3D11_BUFFER_DESC indexBufDesc = {};
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	indexBufDesc.ByteWidth = sizeof(int) * 3*12;

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
	vertexBufDesc.ByteWidth = sizeof(Vector4) * 2*8;

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
	rastDesc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.FrontCounterClockwise = true;

	res = device->CreateRasterizerState(&rastDesc, &rastState);
	res = context->QueryInterface(IID_ID3DUserDefinedAnnotation, (void**)&annotation);

	return res;
}
void CubeComponent::Update(float deltaTime) {
	curRotation += 0.04;
	if (curRotation - 6.28 > 0)
	{
		curRotation = 0;
		if (scaleIteration < 5)
			scaleIteration++;	
		else 
			scaleIteration = -4;			
		curScale = scaleIteration>=0? curScale * 1.1 : curScale / 1.1;
	}
	if (lastScale < curScale && -(lastScale - curScale) > 0.01)
		lastScale += 0.01;
	else if (lastScale > curScale && lastScale - curScale > 0.01)
		lastScale -= 0.01;
	auto pos = Matrix::CreateRotationY(curRotation) * Matrix::CreateScale(lastScale) * Matrix::CreateTranslation(position);//
	auto m = pos * camera->ViewMatrix * camera->ProjMatrix;
	D3D11_MAPPED_SUBRESOURCE res = {};
	context->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	auto dataP = reinterpret_cast<float*>(res.pData);
	memcpy(dataP, &m, sizeof(Matrix));
	context->Unmap(constantBuffer, 0);
}
void CubeComponent::Reload() {

}