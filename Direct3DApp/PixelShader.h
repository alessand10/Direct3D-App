#pragma once
#include "ShaderBase.h"

class PixelShader : public ShaderBase {
public:
	ComPtr<ID3D11PixelShader> pixelShader;
	void initialize(const char* byteCodeFile, ID3D11Device* device);
};
