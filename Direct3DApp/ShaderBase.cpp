#include "ShaderBase.h"
#include <fstream>


SIZE_T ShaderBase::importShaderBytecode(const char* filePath, char** output)
{
	SIZE_T fileLength = 0;
	std::fstream file;
	file.open(filePath, std::fstream::in | std::ios::binary | std::fstream::ate);
	if (file.is_open()) {
		fileLength = file.tellg();
		file.seekg(0, std::ios::beg);
		(*output) = new char[fileLength];
		file.read(*output, fileLength);
	}

	return fileLength;
}
