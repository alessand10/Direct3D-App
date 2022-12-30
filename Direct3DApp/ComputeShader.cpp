#include "ComputeShader.h"
#include "D3DApp.h"
#include "FileReader.h"

ComputeShader::ComputeShader(const char* byteCodeFile, D3DApp* app)
{
	unique_ptr<char[]> byteCode = nullptr;
	SIZE_T byteCodeLength = FileReader::readAllContentsOfFile(byteCodeFile, &byteCode);
	app->getDevice()->CreateComputeShader(byteCode.get(), byteCodeLength, nullptr, &computeShader);
}
