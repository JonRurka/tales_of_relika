#pragma once

#include "opengl.h"

#include <vector>
#include <string>

class Cubemap {
public:

	Cubemap();

	Cubemap(std::vector<std::string> face_path, bool flip = true);

	GLuint Tex() { return m_texture; }

	void Bind();

private:

	GLuint m_texture{ 0 };
	
	const std::string LOG_LOC{ "CUBEMAP" };
	
};