#pragma once
#include "wincodec.h"

class TextureLoader
{
	IWICImagingFactory2* factory;
	ID3D11Device* Device;
	ID3D11DeviceContext* Context;
	
public:
	TextureLoader(ID3D11Device* device, ID3D11DeviceContext* context);
	~TextureLoader();

	HRESULT LoadTextureFromFile(LPCWSTR fileName, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureView, bool generateMips = false, bool useSrgb = false, UINT frameIndex = 0);
};

