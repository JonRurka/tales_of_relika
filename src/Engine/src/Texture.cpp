#include "Texture.h"


#include "stb_image.h"
#include "Utilities.h"
#include "Logger.h"
#include "Framebuffer.h"


Texture::Texture(const std::string path, bool flip)
{
	bool success = true;

	const char* path_ptr = path.c_str();

	GLenum format = GL_RGB;

	//Logger::LogInfo(LOG_POS("NEW"), "Texture path: %s", path_ptr);
		
	if (Utilities::getFileExtension(path) == ".jpg" ||
		Utilities::getFileExtension(path) == ".jpeg") {
		format = GL_RGB;
	}
	else if (Utilities::getFileExtension(path) == ".png") {
		format = GL_RGBA;
	}

	m_name = Utilities::Get_Filename(path);
	m_path = path;

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(flip);
	unsigned char* data = stbi_load(path_ptr, &width, &height, &nrChannels, 0);
	m_data_size = width * height * nrChannels;
	m_raw_data = new unsigned char[m_data_size];
	memcpy(m_raw_data, data, m_data_size);

	m_width = width;
	m_height = height;

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data) {

		if (0 == nrChannels || 4 < nrChannels) { throw std::runtime_error("invalid depth"); }
		GLenum const fmt[] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
		glTexImage2D(GL_TEXTURE_2D, 0, fmt[nrChannels - 1], width, height, 0, fmt[nrChannels - 1], GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		Logger::LogError(LOG_POS("NEW"), "Failed to load texture: %s", path.c_str());
		success = false;
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);

	m_initialized = success;
	m_resizable = false;
}

Texture::Texture(const std::string resource_name, const std::vector<char> file_data, bool flip)
{
	bool success = true;
	GLenum format = GL_RGB;
	if (Utilities::getFileExtension(resource_name) == ".jpg" ||
		Utilities::getFileExtension(resource_name) == ".jpeg") {
		format = GL_RGB;
	}
	else if (Utilities::getFileExtension(resource_name) == ".png") {
		format = GL_RGBA;
	}

	m_name = Utilities::Get_Resource_Filename(resource_name);
	m_path = resource_name;

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(flip);

	stbi_uc* stb_file_data = (stbi_uc*)file_data.data();

	unsigned char* data = stbi_load_from_memory(stb_file_data, file_data.size(), &width, &height, &nrChannels, 0);
	m_data_size = width * height * nrChannels;
	m_raw_data = new unsigned char[m_data_size];
	memcpy(m_raw_data, data, m_data_size);

	m_width = width;
	m_height = height;

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data) {

		if (0 == nrChannels || 4 < nrChannels) { throw std::runtime_error("invalid depth"); }
		GLenum const fmt[] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
		glTexImage2D(GL_TEXTURE_2D, 0, fmt[nrChannels - 1], width, height, 0, fmt[nrChannels - 1], GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		Logger::LogError(LOG_POS("NEW"), "Failed to load texture: %s", resource_name.c_str());
		success = false;
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);

	m_initialized = success;
	m_resizable = false;

}

Texture::Texture(const int width, const int height)
{
	m_width = width;
	m_height = height;
	m_resizable = true;

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind(GLenum texture_unit)
{
	glActiveTexture(texture_unit);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture::Wrap(Wrap_Mode value)
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
	GLint mode;
	switch (value) {
		case Wrap_Mode::REPEAT:
			mode = GL_REPEAT;
			break;

		case Wrap_Mode::CLAMP:
			mode = GL_CLAMP_TO_EDGE;
			break;

		default:
			mode = GL_REPEAT;
			break;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Resize(int width, int height)
{
	if (m_resizable) {

		glDeleteTextures(1, &m_texture);

		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

		for (const auto& elem : m_linked_framebuffers)
		{
			elem->Refresh(false);
		}
	}
}

void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

