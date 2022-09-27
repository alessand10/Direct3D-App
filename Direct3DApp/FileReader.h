#pragma once
#include "Common.h"

class FileReader {
public:
	static size_t readAllContentsOfFile(const char* filePath, class std::unique_ptr<char[]>* output);
};
