#include "VertexShader.h"

VertexShader::VertexShader(const char* byteCodeFile, ID3D11Device* device)
{
	char* byteCode = nullptr;
	SIZE_T byteCodeLength = importShaderBytecode(byteCodeFile, &byteCode);
	device->CreateVertexShader((void*)byteCode, byteCodeLength, nullptr, vertexShader.GetAddressOf());
	delete[] byteCode;
}
