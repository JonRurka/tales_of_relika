#pragma once

#include <string>


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
};

