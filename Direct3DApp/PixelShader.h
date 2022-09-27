#pragma once
#include "ShaderBase.h"

class PixelShader : public ShaderBase {
public:
	ComPtr<ID3D11PixelShader> pixelShader;
	PixelShader(const char* byteCodeFile, class D3DApp* app);
};
