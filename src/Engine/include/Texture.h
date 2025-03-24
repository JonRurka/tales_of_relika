#pragma once

#include "opengl.h"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

class Framebuffer;

class Texture
{
	friend class Framebuffer;
public:

	enum class Type {
		DIFFUSE,
		SPECULAR,
		NORMAL,
		OTHER
	};

	enum class Wrap_Mode {
		REPEAT,
		CLAMP
	};

	Texture(const std::string path);

	Texture(const int width, const int height);

	GLuint Tex() { return m_texture; }

	void type(Type value) { m_type = value; }
	Type type() { return m_type; }

	std::string Name() { return m_name; }

	std::string Path() { return m_path; }

	int Width() { return m_width; }
	int Height() { return m_height; }

	void Resize(int width, int height);

	bool Initialized() { return m_initialized; }

	void Bind();

	void Bind(GLenum texture_unit);

	void Wrap(Wrap_Mode value);



private:
	GLuint m_texture{ 0 };
	Type m_type{ Type::DIFFUSE };
	std::string m_name;
	std::string m_path;
	int m_width{ 0 };
	int m_height{ 0 };
	bool m_initialized{ false };
	bool m_resizable{ false };

	std::vector<Framebuffer*> m_linked_framebuffers;

	const std::string LOG_LOC{ "TEXTURE" };
};

