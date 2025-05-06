#include "Utilities.h"

#include "opengl.h"
#include "zlib.h"

#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>
#include <sstream>

#include <boost/dll.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>

#include "shared_structures.h"
#include "dynamic_compute.h"

#include "Logger.h"

#ifdef _WIN32
#include <Windows.h>
#define BUFSIZE MAX_PATH
#endif

#define ZLIB_CHUNK (256 * 1024)

using namespace DynamicCompute;
using namespace DynamicCompute::Compute;

namespace {
	typedef std::vector<unsigned char> vec_char;
	unsigned char out_buf[ZLIB_CHUNK];

	void vec_write(vec_char& out, const unsigned char* buf, size_t bufLen)
	{
		out.insert(out.end(), buf, buf + bufLen);
	}

	size_t vec_read(const vec_char& in, unsigned char*& inBuf, size_t& inPosition)
	{
		size_t from = inPosition;
		inBuf = const_cast<unsigned char*>(in.data()) + inPosition;
		inPosition += min(ZLIB_CHUNK, in.size() - from);
		return inPosition - from;
	}
}


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
	// Get the full path to the executable
	boost::filesystem::path exe_path = boost::dll::program_location();
	boost::filesystem::path exe_dir = exe_path.parent_path();
	return exe_dir.string() + File_Seperator();

	/*
	// TODO: Linux version

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

	return str + File_Seperator();*/
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

std::string Utilities::Get_Resource_Filename(const std::string& path)
{
	if (path.empty()) {
		return "";  // Return empty string for empty input
	}

	size_t last_slash = path.find_last_of(':');
	size_t last_backslash = path.find_last_of(':');

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

std::string Utilities::Decode_Base64(std::string base64_input)
{
	// https://stackoverflow.com/a/46358091

	using namespace boost::archive::iterators;
	typedef transform_width<binary_from_base64<remove_whitespace
		<std::string::const_iterator> >, 8, 6> ItBinaryT;

	try
	{
		// If the input isn't a multiple of 4, pad with =
		size_t num_pad_chars((4 - base64_input.size() % 4) % 4);
		base64_input.append(num_pad_chars, '=');

		size_t pad_chars(std::count(base64_input.begin(), base64_input.end(), '='));
		std::replace(base64_input.begin(), base64_input.end(), '=', 'A');
		std::string output(ItBinaryT(base64_input.begin()), ItBinaryT(base64_input.end()));
		output.erase(output.end() - pad_chars, output.end());
		return output;
	}
	catch (std::exception const&)
	{
		return std::string("");
	}
}

std::vector<char> Utilities::Read_File_Bytes(const std::string& path)
{
	std::vector<char> res;
	
	// Open the file in binary mode
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		Logger::LogError(LOG_POS("Read_File"), "Failed to open file '%s'.", path.c_str());
		return res;
	}

	// Get the file size (since we're at the end due to ios::ate)
	size_t outSize = file.tellg();
	if (outSize == 0) {
		file.close();
		return res; // Empty file, return nullptr
	}

	// Allocate a char array to hold the file contents
	char* buffer = new char[outSize];

	// Move back to the beginning of the file
	file.seekg(0, std::ios::beg);

	// Read the file contents into the buffer
	file.read(buffer, outSize);
	if (!file) {
		delete[] buffer;
		file.close();
		Logger::LogError(LOG_POS("Read_File"), "Error reading file '%s'.", path.c_str());
		return res;
	}

	// Close the file
	file.close();

	res = std::vector<char>(buffer, buffer + outSize);

	delete[] buffer;

	return res;
}

void Utilities::Read_File_Bytes(const std::string& path, size_t offset, size_t size, char* out_bytes)
{
	//std::vector<char> res;

	// Open the file in binary mode
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		Logger::LogError(LOG_POS("Read_File"), "Failed to open file '%s'.", path.c_str());
		return;
	}

	// Get the file size (since we're at the end due to ios::ate)
	size_t outSize = size;
	if ((offset + outSize) > file.tellg()) {
		file.close();
		return;
	}

	// Allocate a char array to hold the file contents

	char* buffer = out_bytes;
	//char* buffer = new char[outSize];

	// Move back to offset of the file
	file.seekg(offset, std::ios::beg);

	// Read the file contents into the buffer
	file.read(buffer, outSize);
	if (!file) {
		delete[] buffer;
		file.close();
		Logger::LogError(LOG_POS("Read_File"), "Error reading file '%s'.", path.c_str());
		return;
	}

	// Close the file
	file.close();

	//res = std::vector<char>(buffer, buffer + outSize);

	//delete[] buffer;

	//return res;
}

std::string Utilities::Read_File_String(const std::string& path)
{
	std::string res = "";
	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		file.open(path);
		std::stringstream file_stream;
		file_stream << file.rdbuf();
		// close file handlers
		file.close();
		res = file_stream.str();
		file_stream.clear();
	}
	catch (std::ifstream::failure e) {
		//std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		Logger::LogError(LOG_POS("Read_File_String"), "Error reading file '%s'.", path.c_str());
		return res;
	}
	return res;
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

std::vector<unsigned char> Utilities::Compress(std::vector<unsigned char> input, int level)
{
	std::vector<unsigned char> result;

	int ret, flush;
	unsigned have;
	z_stream strm;
	unsigned char* in_buf;

	size_t inPosition = 0; /* position indicator of "in" */

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	ret = deflateInit(&strm, level);
	if (ret != Z_OK)
		return result;

	result.reserve(input.size());

	flush = Z_NO_FLUSH;

	do {
		strm.avail_in = vec_read(input, in_buf, inPosition);

		if (strm.avail_in <= 0)
			break;
		if (strm.avail_in < ZLIB_CHUNK || (input.size() == ZLIB_CHUNK))
		{
			flush = Z_FINISH;
		}
		strm.next_in = in_buf;

		do {
			strm.avail_out = ZLIB_CHUNK;
			strm.next_out = out_buf;
			ret = deflate(&strm, flush);    /* no bad return value */
			assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret) {
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR; /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				return result;
			}
			have = ZLIB_CHUNK - strm.avail_out;
			vec_write(result, out_buf, have);

		} while (strm.avail_out <= 0);

	} while (true);

	(void)inflateEnd(&strm);

	return result;
}

std::vector<unsigned char> Utilities::Decompress(std::vector<unsigned char> input)
{
	std::vector<unsigned char> result;

	int ret, flush;
	unsigned have;
	z_stream strm;
	unsigned char* in_buf;

	size_t inPosition = 0; /* position indicator of "in" */

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	ret = inflateInit(&strm);
	if (ret != Z_OK)
		return result;

	do {
		strm.avail_in = vec_read(input, in_buf, inPosition);

		if (strm.avail_in == 0)
			break;
		strm.next_in = in_buf;

		/* run inflate() on input until output buffer not full */
		do {
			strm.avail_out = ZLIB_CHUNK;
			strm.next_out = out_buf;
			ret = inflate(&strm, Z_NO_FLUSH);
			assert(ret != Z_STREAM_ERROR); /* state not clobbered */
			switch (ret) {
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR; /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				return result;
			}
			have = ZLIB_CHUNK - strm.avail_out;
			vec_write(result, out_buf, have);
		} while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	} while (ret != Z_STREAM_END);

	/* clean up and return */
	(void)inflateEnd(&strm);

	return result;
}

OpenCL_Device_Info Utilities::Get_Recommended_Device()
{
	std::vector<OpenCL_Device_Info> devices;
	std::vector<Platform> platforms = ComputeInterface::GetSupportedPlatforms_OpenCL();
	//printf("Reported Platforms: %i\n", (int)platforms.size());
	for (const auto& plt : platforms) {
		//printf("Platform Name: %s (%llX)\n", plt.name, (long long)plt.platform);
		std::vector<OpenCL_Device_Info> plt_devices = ComputeInterface::GetSupportedDevices_OpenCL(plt);
		for (const auto& device : plt_devices) {
			devices.push_back(device);
			//printf("\tDevice: %s, GPU: %i, CPU: %i, Comp Units: %i\n", device.name, (int)device.is_type_GPU, (int)device.is_type_CPU, device.num_compute_units);
		}
	}
	//printf("\n");

	OpenCL_Device_Info picked_device;
	picked_device.num_compute_units = 0;
	int max_comp = 0;
	for (const auto& elem : devices) {
		if (elem.num_compute_units > picked_device.num_compute_units) {
			picked_device = elem;
		}
	}
	//picked_device = devices[1];
	return picked_device;
}


