#include "LightTexturedComponent.h"

struct ConstantData {
	Matrix WorldViewProj;
	Matrix World;
	Vector4 ViewerPos;
};

struct LightData {
	Vector4 Direction;
	Vector4 Color;
	Vector4 KaSpecPowKsX;
};

LightTexturedComponent::LightTexturedComponent(ID3D11Device* device, ID3D11DeviceContext* context, Camera* camera, LPCWSTR inFileName, LPCWSTR inTexName):camera(camera),context(context),device(device) {
	textureName = inTexName;
	texLoader = new TextureLoader(device, context);
	position = Vector3(0,0,0);
}
void LightTexturedComponent::DestroyResources() {
	if (texSrv) texSrv->Release();
	if (rastState) rastState->Release();
	if (layout) layout->Release();
	if (vertexShader) vertexShader->Release();
	if (VertexShaderByteCode) VertexShaderByteCode->Release();
	if (pixelShader) pixelShader->Release();
	if (PixelShaderByteCode) PixelShaderByteCode->Release();
	if (vertices) vertices->Release();
}
void LightTexturedComponent::Draw(float deltaTime) {
	UINT strides = 48;
	UINT offsets = 0;

	context->IASetInputLayout(layout);									
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(0, 1, &vertices, &strides, &offsets);

	context->VSSetConstantBuffers(0, 1, &constantBuffer);	
	context->PSSetConstantBuffers(1, 1, &lightBuffer);
	context->PSSetShaderResources(0, 1, &texSrv);
	context->PSSetSamplers(0, 1, &sampler);

	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	context->RSSetState(rastState);

	context->DrawIndexed(36, 0, 0);
}
HRESULT LightTexturedComponent::Initialize() {
	HRESULT res;

	res = CreateShader(L"ObjModelShader.hlsl", "VSMain", "vs_5_0", &VertexShaderByteCode, nullptr);
	if (FAILED(res)) return res;

	res = CreateShader(L"ObjModelShader.hlsl", "PSMain", "ps_5_0", &PixelShaderByteCode, nullptr);
	if (FAILED(res)) return res;

	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		//D3D11_INPUT_ELEMENT_DESC { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0},		
		D3D11_INPUT_ELEMENT_DESC { "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		//D3D11_INPUT_ELEMENT_DESC { "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		//D3D11_INPUT_ELEMENT_DESC { "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		D3D11_INPUT_ELEMENT_DESC { "TEXTCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	res = device->CreateVertexShader(VertexShaderByteCode->GetBufferPointer(), VertexShaderByteCode->GetBufferSize(), nullptr, &vertexShader);
	if (FAILED(res)) return res;
	res = device->CreatePixelShader(PixelShaderByteCode->GetBufferPointer(), PixelShaderByteCode->GetBufferSize(), nullptr, &pixelShader);
	if (FAILED(res)) return res;

	res = device->CreateInputLayout(inputElements, 3, VertexShaderByteCode->GetBufferPointer(), VertexShaderByteCode->GetBufferSize(), &layout);

	//points = new Vector4[] {Vector4(1.5f, 1.5f, 1.5f, 1.0f),  Vector4(0.0f,1.0f,0.0f,1.0f), Vector4(1.0f, 0.0f, 0.0f, 0.0f),	//0
	//	Vector4(-1.5f, -1.5f, 1.5f, 1.0f), Vector4(1.0f,1.0f,1.0f,1.0f),	Vector4(0.0f, 1.0f, 0.0f, 0.0f),	//1
	//	Vector4(1.5f, -1.5f, 1.5f, 1.0f), Vector4(1.0f,1.0f,1.0f,1.0f), Vector4(1.0f, 1.0f, 0.0f, 0.0f),		//2
	//	Vector4(-1.5f, 1.5f, 1.5f, 1.0f), Vector4(1.0f,1.0f,1.0f,1.0f), Vector4(0.0f, 0.0f, 0.0f, 0.0f),		//3
	//	Vector4(1.5f, 1.5f, -1.5f, 1.0f), Vector4(1.0f,1.0f,1.0f,1.0f), Vector4(0.0f, 0.0f, 0.0f, 0.0f),		//4
	//	Vector4(1.5f, -1.5f, -1.5f, 1.0f), Vector4(1.0f,1.0f,1.0f,1.0f), Vector4(0.0f, 1.0f, 0.0f, 0.0f),		//5
	//	Vector4(-1.5f, 1.5f, -1.5f, 1.0f), Vector4(1.0f,1.0f,1.0f,1.0f), Vector4(1.0f, 0.0f, 0.0f, 0.0f),		//6
	//	Vector4(-1.5f, -1.5f, -1.5f, 1.0f), Vector4(1.0f,1.0f,1.0f,1.0f), Vector4(1.0f, 1.0f, 0.0f, 0.0f),
	//};		//7
	//indeces = new int[] { 0,1,2, 1,0,3, 4,2,5, 2,4,0, 6,5,7, 5,6,4, 3,7,1, 7,3,6, 4,3,0, 3,4,6, 2,7,5, 7,2,1 };

	points = new Vector4[]{
		Vector4(-1.0f, 2.0f, -1.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f),
		Vector4(1.0f, 2.0f, -1.0f, 1.0f),  Vector4(0.0f, 1.0f, 0.0f, 1.0f) ,Vector4(1.0f, 0.0f, 1.0f, 1.0f),
		Vector4(1.0f, 2.0f, 1.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f),Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4(-1.0f, 2.0f, 1.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f),Vector4(0.0f, 1.0f, 1.0f, 1.0f),
		Vector4(-1.0f, 0.0f, -1.0f, 1.0f),Vector4(0.0f, -1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f),
		Vector4(1.0f, 0.0f, -1.0f, 1.0f), Vector4(0.0f, -1.0f, 0.0f, 1.0f), Vector4(1.0f, 0.0f, 1.0f, 1.0f),
		Vector4(1.0f, 0.0f, 1.0f, 1.0f), Vector4(0.0f, -1.0f, 0.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4(-1.0f, 0.0f, 1.0f, 1.0f), Vector4(0.0f, -1.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 1.0f, 1.0f),
		Vector4(-1.0f, 0.0f, 1.0f, 1.0f), Vector4(-1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f),
		Vector4(-1.0f, 0.0f, -1.0f, 1.0f), Vector4(-1.0f, 0.0f, 0.0f, 1.0f), Vector4(1.0f, 0.0f, 1.0f, 1.0f),
		Vector4(-1.0f, 2.0f, -1.0f, 1.0f), Vector4(-1.0f, 0.0f, 0.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4(-1.0f, 2.0f, 1.0f, 1.0f), Vector4(-1.0f, 0.0f, 0.0f, 1.0f) , Vector4(0.0f, 1.0f, 1.0f, 1.0f),
		Vector4(1.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f),
		Vector4(1.0f, 0.0f, -1.0f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(1.0f, 0.0f, 1.0f, 1.0f),
		Vector4(1.0f, 2.0f, -1.0f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4(1.0f, 2.0f, 1.0f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 1.0f, 1.0f),
		Vector4(-1.0f, 0.0f, -1.0f, 1.0f), Vector4(0.0f, 0.0f, -1.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f),
		Vector4(1.0f, 0.0f, -1.0f, 1.0f), Vector4(0.0f, 0.0f, -1.0f, 1.0f), Vector4(1.0f, 0.0f, 1.0f, 1.0f),
		Vector4(1.0f, 2.0f, -1.0f, 1.0f), Vector4(0.0f, 0.0f, -1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4(-1.0f, 2.0f, -1.0f, 1.0f), Vector4(0.0f, 0.0f, -1.0f, 1.0f),  Vector4(0.0f, 1.0f, 1.0f, 1.0f),
		Vector4(-1.0f, 0.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f),  Vector4(0.0f, 0.0f, 1.0f, 1.0f),
		Vector4(1.0f, 0.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 0.0f, 1.0f, 1.0f),
		Vector4(1.0f, 2.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f),  Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4(-1.0f, 2.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(0.0f, 1.0f, 1.0f, 1.0f) };
	indeces = new int[]
	{
		3,1,0,
		2,1,3,
		6,4,5,
		7,4,6,
		11,9,8,
		10,9,11,
		14,12,13,
		15,12,14,
		19,17,16,
		18,17,19,
		22,20,21,
		23,20,22
	};

	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;									// то, как часто будет обновляться вершинный буфер
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;							//куда буфер может быть привязан
	vertexBufDesc.CPUAccessFlags = 0;											//0 -если не хотим чтения и записи с цпу
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = sizeof(Vector4) * 24*3;	//размер буфера в байтах

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = points;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	res = device->CreateBuffer(&vertexBufDesc, &vertexData, &vertices);

	//индексный буфер
	D3D11_BUFFER_DESC indexBufDesc = {};
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	indexBufDesc.ByteWidth = sizeof(int) * 3*12;//indeces length

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indeces;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	res = device->CreateBuffer(&indexBufDesc, &indexData, &indexBuffer);

	D3D11_BUFFER_DESC constBufDesc = {};
	constBufDesc.Usage = D3D11_USAGE_DEFAULT;
	constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufDesc.CPUAccessFlags = 0;
	constBufDesc.MiscFlags = 0;
	constBufDesc.StructureByteStride = 0;
	constBufDesc.ByteWidth = sizeof(ConstantData);	

	res = device->CreateBuffer(&constBufDesc, nullptr, &constantBuffer);

	D3D11_BUFFER_DESC lightBufDesc = {};
	lightBufDesc.Usage = D3D11_USAGE_DEFAULT;
	lightBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufDesc.CPUAccessFlags = 0;
	lightBufDesc.MiscFlags = 0;
	lightBufDesc.StructureByteStride = 0;
	lightBufDesc.ByteWidth = sizeof(ConstantData);

	res = device->CreateBuffer(&lightBufDesc, nullptr, &lightBuffer);

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.FrontCounterClockwise = true;


	res = device->CreateRasterizerState(&rastDesc, &rastState);

	texLoader->LoadTextureFromFile(textureName, texture, texSrv, true, false); //грузим нашу текстуру

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

	res = device->CreateSamplerState(&samplerDesc, &sampler); //создаем семплер стейт для текстуры
}
void LightTexturedComponent::Update(float deltaTime) {	
	auto data = ConstantData{};	
	auto pos = camera->GetPosition();
	data.World = Matrix::CreateTranslation(position);
	data.WorldViewProj = data.World * camera->GetCameraMatrix();
	data.ViewerPos = Vector4(pos.x,pos.y,pos.z,1.0f);
	x = cos(curAngle)*10;
	y = sin(curAngle)*10;
	curAngle = curAngle + 0.01;
	std::cout << curAngle <<std::endl;
	if (curAngle == 360)
	{
		curAngle = 0;
	}
	auto lightData = LightData{};
	lightData.Color = Vector4(255.0f, 255.0f, 255.0f, 1.0f);
	lightData.Direction = Vector4(x, 100.0f, y, 1.0f);
	lightData.KaSpecPowKsX = Vector4(0.5f, 0.5f, 0.2f, 1.0f);
	context->UpdateSubresource(constantBuffer, 0, nullptr, &data, 0, 0);
	context->UpdateSubresource(lightBuffer, 0, nullptr, &lightData, 0, 0);
}
void LightTexturedComponent::Reload() {

}

HRESULT LightTexturedComponent::CreateShader(LPCWSTR fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** vertexBC, D3D_SHADER_MACRO* shaderMacros) {
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