#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define ZLIB_DEFAULT_COMPRESSION (-1)

#define Remove_If_Found(vec, target)						\
do {														\
	auto it = std::find(vec.begin(), vec.end(), target);	\
	if (it != vec.end()) {									\
		vec.erase(it);										\
	}														\
}while(false)

class Utilities
{
public:

	static std::string File_Seperator();

	static std::string Get_Root_Directory();

	static std::string toLowerCase(std::string str);

	static std::string getFileExtension(const std::string& filePath);

	static std::string Get_Filename(const std::string& path);

	static double Get_Time();

	static int Is_Extension_Supported(const std::string extension);

	static std::vector<glm::vec4> vec2_to_vec4_arr(std::vector<glm::vec2> arr);

	static std::vector<glm::vec4> vec3_to_vec4_arr(std::vector<glm::vec3> arr);

	static std::vector<glm::vec3> vec4_to_vec3_arr(std::vector<glm::vec4> arr);

	static std::vector<unsigned char> Compress(std::vector<unsigned char> input, int level = ZLIB_DEFAULT_COMPRESSION);

	static std::vector<unsigned char> Decompress(std::vector<unsigned char> input);

};

