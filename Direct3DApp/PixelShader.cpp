#include "PixelShader.h"
#include "D3DApp.h"
#include "FileReader.h"

PixelShader::PixelShader(const char* byteCodeFile, D3DApp* app)
{
	unique_ptr<char[]> byteCode = nullptr;
	SIZE_T byteCodeLength = FileReader::readAllContentsOfFile(byteCodeFile, &byteCode);
	app->getDevice()->CreatePixelShader(byteCode.get(), byteCodeLength, nullptr, &pixelShader);
}
