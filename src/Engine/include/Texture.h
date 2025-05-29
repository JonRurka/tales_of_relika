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

	enum class Dimensions {
		TEXTURE_2D,
		RENDER_TEXTURE_2D,
		TEXTURE_2D_ARRAY,
	};

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

	Texture(const std::string path, bool flip = true);

	Texture(const std::string resource_name, const std::vector<char> data, bool flip = true);

	Texture(const std::vector<Texture*> textures);

	Texture(const int width, const int height);

	GLuint Tex() { return m_texture; }

	unsigned char* Data() { return m_raw_data; }
	size_t Data_Size() { return m_data_size; }

	void type(Type value) { m_type = value; }
	Type type() { return m_type; }

	GLenum Target_Type();

	GLenum Format() { return m_format; }

	std::string Name() { return m_name; }

	std::string Path() { return m_path; }

	int Width() { return m_width; }
	int Height() { return m_height; }

	void Resize(int width, int height);

	bool Initialized() { return m_initialized; }

	void Bind();

	void Bind(GLenum texture_unit);

	void Wrap(Wrap_Mode value);

	static Texture* Create_Texture2D_Array(std::vector<std::string> resource_names, bool flip = true);

private:
	GLuint m_texture{ 0 };
	Type m_type{ Type::DIFFUSE };
	Dimensions m_dim{ Dimensions::TEXTURE_2D };
	GLenum m_format{ 0 };
	std::string m_name;
	std::string m_path;
	int m_width{ 0 };
	int m_height{ 0 };
	bool m_initialized{ false };
	bool m_resizable{ false };
	unsigned char* m_raw_data{nullptr};
	size_t m_data_size{ 0 };

	std::vector<Framebuffer*> m_linked_framebuffers;

	inline static const std::string LOG_LOC{ "TEXTURE" };
};

