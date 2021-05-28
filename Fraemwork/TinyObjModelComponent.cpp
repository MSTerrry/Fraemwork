#include "TinyObjModelComponent.h"
using namespace DirectX::SimpleMath;

#pragma pack(4)
struct ConstDataBuf {
	Matrix WVP;
};

ConstDataBuf data = { };

TinyObjModelComponent::TinyObjModelComponent(ID3D11Device* device, ID3D11DeviceContext* context, Camera* camera, const char* fileName):device(device),context(context),camera(camera)
{
	modelName = fileName;
}
HRESULT TinyObjModelComponent::CreateShader(LPCWSTR fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** vertexBC, D3D_SHADER_MACRO* shaderMacros) {
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
HRESULT TinyObjModelComponent::Initialize() {
	HRESULT res;
	res = CreateShader(L"TinyModelShader.hlsl", "VSMain", "vs_5_0", &VertexShaderByteCode, nullptr);
	if (FAILED(res)) return res;

	;
	res = CreateShader(L"TinyModelShader.hlsl", "PSMain", "ps_5_0", &PixelShaderByteCode, nullptr);
	if (FAILED(res)) return res;

	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		D3D11_INPUT_ELEMENT_DESC { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	res = device->CreateVertexShader(VertexShaderByteCode->GetBufferPointer(), VertexShaderByteCode->GetBufferSize(), nullptr, &vertexShader);
	if (FAILED(res)) return res;
	res = device->CreatePixelShader(PixelShaderByteCode->GetBufferPointer(), PixelShaderByteCode->GetBufferSize(), nullptr, &pixelShader);
	if (FAILED(res)) return res;
	
	
	//ObjLoader->LoadTinyModel(modelName, vBuf, nBuf, tBuf, strBuf, Materials, Shapes, elemCount);
	D3D11_BUFFER_DESC descBuf = {};
	vBuf->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = descBuf.ByteWidth / sizeof(Vector3);
		

	res = device->CreateShaderResourceView(vBuf, &srvDesc, &vSrv);

	nBuf->GetDesc(&descBuf);
	srvDesc.Buffer.NumElements = descBuf.ByteWidth / sizeof(Vector3);
	res = device->CreateShaderResourceView(nBuf, &srvDesc, &nSrv);

	tBuf->GetDesc(&descBuf);
	srvDesc.Buffer.NumElements = descBuf.ByteWidth / sizeof(Vector2);
	srvDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	res = device->CreateShaderResourceView(tBuf, &srvDesc, &tSrv);

	strBuf->GetDesc(&descBuf);
	srvDesc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	res = device->CreateShaderResourceView(strBuf, &srvDesc, &strSrv);

	D3D11_BUFFER_DESC constBuf = {};
	constBuf.Usage = D3D11_USAGE_DEFAULT;
	constBuf.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBuf.CPUAccessFlags = 0;
	constBuf.MiscFlags = 0;
	constBuf.StructureByteStride = 0;
	constBuf.ByteWidth = sizeof(ConstDataBuf);

	res = device->CreateBuffer(&constBuf, nullptr, &constantBuffer);
	if (FAILED(res)) return res;

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.MultisampleEnable = true;

	res = device->CreateRasterizerState(&rastDesc, &rastState);
	if (FAILED(res)) return res;


	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;

	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;	
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;

	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

	res = device->CreateBlendState(&blendDesc, &blendState); if (FAILED(res)) return res;

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
	res = device->CreateSamplerState(&samplerDesc, &sampler); if (FAILED(res)) return res;
	int totalCount = 0;
	for (int i = 0; i < elemCount; i++)
	{
		//totalCount += Shapes[i].Count;
	}

	int* indexes = new int[totalCount];
	for (int i = 0; i < totalCount; i++)
		indexes[i] = i;

	D3D11_BUFFER_DESC indexDesc = {};
	indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.CPUAccessFlags = 0;
	indexDesc.MiscFlags = 0;
	indexDesc.StructureByteStride = 0;
	indexDesc.ByteWidth = sizeof(int) * totalCount;

	D3D11_SUBRESOURCE_DATA resData = { };
	resData.pSysMem = indexes;
	resData.SysMemPitch = 0;
	resData.SysMemSlicePitch = 0;

	res = device->CreateBuffer(&indexDesc, &resData, &indexBuffer); if (FAILED(res)) return res;

	delete[] indexes;
	return res;
}

void TinyObjModelComponent::Update(float deltaTime) {

	auto world = Transform * Matrix::CreateTranslation(Position);
	auto proj = world * camera->GetCameraMatrix();

	data.WVP = proj;
	context->UpdateSubresource(constantBuffer, 0, nullptr, &data, 0, 0);
}
void TinyObjModelComponent::Draw(float deltaTime) {
	ID3D11RasterizerState* oldState;

	context->RSGetState(&oldState);
	context->RSSetState(rastState);

	context->IASetInputLayout(nullptr);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);

	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	ID3D11ShaderResourceView* srvs[] = {vSrv,nSrv,tSrv,strSrv};
	context->VSSetShaderResources(0, 4, srvs);

	context->VSSetConstantBuffers(0, 1, &constantBuffer);
	//context->PSSetShaderResources(0, 1, &sampler);
	context->PSSetSamplers(0, 1, &sampler);
	for (int i = 0; i < elemCount; i++)
	{
		auto shape = Shapes[i];
		auto material = Materials[i];
		
		//context->PSSetShaderResources(0, 1, &material.DiffSrv);
		//context->DrawIndexed(shape.Count, shape.StartIndex, 0);
	}
	context->RSSetState(oldState);
	if (oldState)
		oldState->Release();
}
void TinyObjModelComponent::DestroyResources() {

}
void TinyObjModelComponent::Reload() {

}