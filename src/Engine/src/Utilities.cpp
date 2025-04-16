#include "Utilities.h"

#include "opengl.h"
#include "zlib.h"

#include <cctype>
#include <algorithm>

#ifdef _WIN32
#include <Windows.h>
#define BUFSIZE MAX_PATH
#endif

#define ZLIB_CHUNK (256 * 1024)

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

	return str + File_Seperator();
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
