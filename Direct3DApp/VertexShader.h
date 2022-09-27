#pragma once
#include "ShaderBase.h"

class VertexShader : public ShaderBase {
public:
	ComPtr<ID3D11VertexShader> vertexShader;
	VertexShader(const char* byteCodeFile, class D3DApp* app);
};