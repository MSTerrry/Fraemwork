#pragma once
#include <d3d11.h>
#include <iostream>

class TinyShape {
public:
	float StartIndex;
	int Count;
	int MaterialInd;
};

class TinyMaterial {
public:
	std::string TexName;
	ID3D11Texture2D* DiffuseTexture = nullptr;
	ID3D11ShaderResourceView* ResourceView = nullptr;
};