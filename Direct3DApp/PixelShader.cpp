#include "PixelShader.h"

void PixelShader::initialize(const char* byteCodeFile, ID3D11Device* device)
{
	char* byteCode = nullptr;
	SIZE_T byteCodeLength = importShaderBytecode(byteCodeFile, &byteCode);
	device->CreatePixelShader(byteCode, byteCodeLength, nullptr, &pixelShader);
	delete[] byteCode;
}
