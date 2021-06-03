#pragma once
#include <iostream>
#include <d3d11.h>
#include <DirectXMath.h>
#include "SimpleMath.h"
#include "TextureLoader.h"
#include <vector>
using namespace DirectX::SimpleMath;

class TinyShape {
public:
	float StartIndex;
	int Count;
	int MaterialInd;
};

class TinyMaterial {
public:
	std::string TexName;
	ID3D11Texture2D *DiffuseTexture = nullptr;
	ID3D11ShaderResourceView *ResourceView = nullptr;
};

class ObjLoader {
	ID3D11Device* device = nullptr;
	TextureLoader* textureLoader = nullptr;
public:
	ObjLoader(ID3D11Device* dev, ID3D11DeviceContext* context);
	void LoadTinyModel(const char* fileName, ID3D11Buffer*& vBuf, ID3D11Buffer*& nBuf, ID3D11Buffer*& tBuf,
		ID3D11Buffer*& strBuf, TinyShape*& outShapes, TinyMaterial*& outMaterials, int& elemCount);
	void LoadObjModel(const char* fileName,ID3D11Buffer*& vertices, int& elemCount);
};