#include "FileReader.h"
#include <fstream>

/**
 * @brief Reads a file located at some path and stores it in an array.
 * @note The output array is created on the heap using a smart pointer to prevent memory leaks.
 * @param filePath A string specifying the path of the file to read.
 * @param output A pointer to a unique ptr, output will be passed through this argument.
 * @return The file size.
 */
size_t FileReader::readAllContentsOfFile(const char* filePath, unique_ptr<char[]>* output)
{
	size_t fileLength = 0;
	std::fstream file;
	file.open(filePath, std::fstream::in | std::ios::binary | std::fstream::ate);
	if (file.is_open()) {
		fileLength = file.tellg();
		file.seekg(0, std::ios::beg);
		*output = std::make_unique<char[]>(fileLength);
		file.read((*output).get(), fileLength);
	}

	return fileLength;
}
