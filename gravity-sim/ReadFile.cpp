#include "ReadFile.h"

#include <fstream>

std::string ReadFile(const std::string &filename, bool &succ)
{
	succ = true;

	std::ifstream file_stream(filename, std::ios_base::in);

	if (file_stream.is_open())
		return std::string((std::istreambuf_iterator<char>(file_stream)),
		std::istreambuf_iterator<char>());

	succ = false;
	return std::string();
}