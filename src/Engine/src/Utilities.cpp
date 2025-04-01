#include "Utilities.h"

#include "opengl.h"

#include <cctype>
#include <algorithm>
#include <Windows.h>
#define BUFSIZE MAX_PATH

std::string Utilities::File_Seperator()
{
#ifdef _WIN32
	return "\\";
#else
	return "/";
#endif
}

std::string Utilities::Get_Root_Directory()
{
	TCHAR Buffer[BUFSIZE];
	DWORD dwRet;

	dwRet = GetCurrentDirectory(BUFSIZE, Buffer);

	//_tprintf(TEXT("Current dir: %s \n"), Buffer);

	std::string str;

#ifndef UNICODE
	str = Buffer;
#else
	std::wstring wStr = Buffer;
	str = std::string(wStr.begin(), wStr.end());
#endif

	//std::cout << str << std::endl; //<-- should work!

	return str + "\\";
}

std::string Utilities::toLowerCase(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return str;
}

std::string Utilities::getFileExtension(const std::string& filePath)
{
	// Find the last occurrence of the dot
	size_t dotPos = filePath.find_last_of('.');

	// If no dot is found or dot is the last character, return empty string
	if (dotPos == std::string::npos || dotPos == filePath.length() - 1) {
		return "";
	}

	// Return substring from the dot to the end
	return toLowerCase(filePath.substr(dotPos));
}

std::string Utilities::Get_Filename(const std::string& path)
{
	if (path.empty()) {
		return "";  // Return empty string for empty input
	}

	size_t last_slash = path.find_last_of('/');
	size_t last_backslash = path.find_last_of('\\');

	// If no separators found, return the whole string
	if (last_slash == std::string::npos && last_backslash == std::string::npos) {
		return path;
	}

	// Find the rightmost separator
	size_t filename_pos = 0;
	if (last_slash != std::string::npos && last_backslash != std::string::npos) {
		filename_pos = max(last_slash, last_backslash);
	}
	else if (last_slash != std::string::npos) {
		filename_pos = last_slash;
	}
	else {
		filename_pos = last_backslash;
	}

	return path.substr(filename_pos + 1);
}

double Utilities::Get_Time()
{
	return glfwGetTime();
}

int Utilities::Is_Extension_Supported(const std::string extension)
{
	GLint numExtensions = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	for (GLint i = 0; i < numExtensions; i++) {
		const char* ext = (const char*)glGetStringi(GL_EXTENSIONS, i);
		if (ext && strcmp(ext, extension.c_str()) == 0) {
			return 1; // Supported
		}
	}
	return 0; // Not supported
}

std::vector<glm::vec4> Utilities::vec2_to_vec4_arr(std::vector<glm::vec2> arr)
{
	std::vector<glm::vec4> res;
	res.reserve(arr.size());

	for (const auto& elem : arr) {
		res.push_back(glm::vec4(elem, 0.0f, 0.0f));
	}

	return res;
}

std::vector<glm::vec4> Utilities::vec3_to_vec4_arr(std::vector<glm::vec3> arr)
{
	std::vector<glm::vec4> res;
	res.reserve(arr.size());

	for (const auto& elem : arr) {
		res.push_back(glm::vec4(elem, 0.0f));
	}

	return res;
}

std::vector<glm::vec3> Utilities::vec4_to_vec3_arr(std::vector<glm::vec4> arr)
{
	std::vector<glm::vec3> res;
	res.reserve(arr.size());

	for (const auto& elem : arr) {
		res.push_back(glm::vec3(elem));
	}

	return res;
}
