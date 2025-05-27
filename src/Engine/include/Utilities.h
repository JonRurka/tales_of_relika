#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shared_structures.h"
#include "dynamic_compute.h"

using namespace DynamicCompute;
using namespace DynamicCompute::Compute;

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

	static std::string Get_Resource_Filename(const std::string& path);
	
	static std::string Decode_Base64(std::string base64_input);

	static std::vector<char> Read_File_Bytes(const std::string& path);

	static void Read_File_Bytes(const std::string& path, size_t offset, size_t size, char* out_bytes);

	static std::string Read_File_String(const std::string& path);

	static double Get_Time();

	static int Is_Extension_Supported(const std::string extension);

	static std::vector<glm::vec4> vec2_to_vec4_arr(std::vector<glm::vec2> arr);

	static std::vector<glm::vec4> vec3_to_vec4_arr(std::vector<glm::vec3> arr);

	static std::vector<glm::vec3> vec4_to_vec3_arr(std::vector<glm::vec4> arr);

	static std::vector<unsigned char> Compress(std::vector<unsigned char> input, int level = ZLIB_DEFAULT_COMPRESSION);

	static std::vector<unsigned char> Decompress(std::vector<unsigned char> input);

	static OpenCL_Device_Info Get_Recommended_Device();

	static int Hash_Chunk_Coord(glm::ivec3 coord) { return Hash_Chunk_Coord(coord.x, coord.y, coord.z); }

	static int Hash_Chunk_Coord(int x, int y, int z);

	enum class Vertex_Limit_Mode : int 
	{
		Max = UINT16_MAX,
		Chunk_Max = 40000
	};

private:
	inline static const std::string LOG_LOC{ "UTILITIES" };

};

