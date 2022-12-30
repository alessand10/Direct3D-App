#pragma once
#include "ShaderBase.h"

class ComputeShader : public ShaderBase {
public:
	ComPtr<ID3D11ComputeShader> computeShader;
	ComputeShader(const char* byteCodeFile, class D3DApp* app);
};