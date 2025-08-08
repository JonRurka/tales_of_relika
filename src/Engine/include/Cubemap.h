#pragma once

#include "opengl.h"

#include <vector>
#include <string>

class Cubemap {
public:

	Cubemap();

	Cubemap(std::vector<std::string> face_path, bool flip = true);

	~Cubemap() {
		Dispose();
	}

	GLuint Tex() { return m_texture; }

	void Bind();

	void Dispose();

private:

	GLuint m_texture{ 0 };
	bool m_initialized{ false };
	
	const std::string LOG_LOC{ "CUBEMAP" };
	
};