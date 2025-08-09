#pragma once

#include "opengl.h"

#include <vector>
#include <string>
#include <memory>

class Cubemap {
public:
	typedef std::shared_ptr<Cubemap> Shared;
	typedef std::weak_ptr<Cubemap> Weak;

	Cubemap() = delete;

	Cubemap(std::vector<std::string> face_path, bool flip = true);

	~Cubemap() {
		Dispose();
	}

	static Shared Create(std::vector<std::string> face_path, bool flip = true);

	GLuint Tex() { return m_texture; }

	void Bind();

	void Dispose();

private:

	GLuint m_texture{ 0 };
	bool m_initialized{ false };
	
	const std::string LOG_LOC{ "CUBEMAP" };
	
};