#include "VertexShader.h"
#include "D3DApp.h"
#include "FileReader.h"

VertexShader::VertexShader(const char* byteCodeFile, D3DApp* app)
{
	unique_ptr<char[]> byteCode = nullptr;
	SIZE_T byteCodeSize = FileReader::readAllContentsOfFile(byteCodeFile, &byteCode);
	app->getDevice()->CreateVertexShader(byteCode.get(), byteCodeSize, nullptr, vertexShader.GetAddressOf());
}