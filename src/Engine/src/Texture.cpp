#include "Texture.h"


#include "stb_image.h"
#include "Utilities.h"
#include "Logger.h"
#include "Framebuffer.h"
#include "Resources.h"


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

	m_dim = Dimensions::TEXTURE_2D;
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
		m_format = fmt[nrChannels - 1];
		glTexImage2D(GL_TEXTURE_2D, 0, m_format, width, height, 0, fmt[nrChannels - 1], GL_UNSIGNED_BYTE, data);
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

	m_dim = Dimensions::TEXTURE_2D;
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
		m_format = fmt[nrChannels - 1];
		glTexImage2D(GL_TEXTURE_2D, 0, m_format, width, height, 0, fmt[nrChannels - 1], GL_UNSIGNED_BYTE, data);
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

Texture::Texture(const std::vector<Texture*> textures)
{
	m_dim = Dimensions::TEXTURE_2D_ARRAY;
	m_name = "texture_array";
	m_path = m_name;


	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	

	GLenum const fmt_arr[] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };

	for (int layer = 0; layer < textures.size(); layer++)
	{
		
		GLenum fmt = textures[layer]->Format();
		GLenum fmt_int = GL_RGB8;
		switch (fmt) {
		case GL_RGB:
			fmt_int = GL_RGB8;
			break;
		case GL_RGBA:
			fmt_int = GL_RGBA8;
			break;
		}

		if (layer == 0) {
			m_width = textures[layer]->Width();
			m_height = textures[layer]->Height();
			glTexStorage3D(GL_TEXTURE_2D_ARRAY,
				10,					//5 mipmaps
				fmt_int,			//Internal format
				m_width, m_height,	//width,height
				textures.size()		//Number of layers
			);
		}
		else {
			int img_width = textures[layer]->Width();
			int img_height = textures[layer]->Height();
			if (img_width != m_width ||
				img_height != m_height) {
				throw std::runtime_error("Width and Height must match");
			}
		}

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
			0,                      //Mipmap number
			0, 0, layer,			//xoffset, yoffset, zoffset
			m_width, m_height, 1,   //width, height, depth
			fmt,					//format
			GL_UNSIGNED_BYTE,       //type
			textures[layer]->Data());

	}

	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

Texture::Texture(const int width, const int height)
{
	m_dim = Dimensions::RENDER_TEXTURE_2D;
	m_width = width;
	m_height = height;
	m_resizable = true;
	m_name = "render_texture";

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

GLenum Texture::Target_Type()
{
	GLenum type = GL_TEXTURE_2D;

	switch (m_dim) {
	case Dimensions::TEXTURE_2D:
		type = GL_TEXTURE_2D;
		break;
	case Dimensions::RENDER_TEXTURE_2D:
		type = GL_TEXTURE_2D;
		break;
	case Dimensions::TEXTURE_2D_ARRAY:
		type = GL_TEXTURE_2D_ARRAY;
		break;
	}

	return type;
}

void Texture::Wrap(Wrap_Mode value)
{
	glBindTexture(Target_Type(), m_texture);
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
	glTexParameteri(Target_Type(), GL_TEXTURE_WRAP_S, mode);
	glTexParameteri(Target_Type(), GL_TEXTURE_WRAP_T, mode);

	glBindTexture(Target_Type(), 0);
}

Texture* Texture::Create_Texture2D_Array(std::vector<std::string> resource_names, bool flip)
{
	std::vector<Texture*> textures;
	textures.reserve(resource_names.size());

	for (const auto& res_name : resource_names) {
		if (!Resources::Has_Texture(res_name)) {
			Logger::LogError(LOG_POS("Create_Texture2D_Array"), "Failed to load texture %s into texture array.", res_name.c_str());
			continue;
		}

		Texture* tex = Resources::Get_Texture(res_name);
		textures.push_back(tex);
	}

	return new Texture(textures);
}

void Texture::Resize(int width, int height)
{
	if (m_resizable) {

		glDeleteTextures(1, &m_texture);

		glGenTextures(1, &m_texture);
		glBindTexture(Target_Type(), m_texture);

		glTexImage2D(Target_Type(), 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(Target_Type(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(Target_Type(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(Target_Type(), 0);

		for (const auto& elem : m_linked_framebuffers)
		{
			elem->Refresh(false);
		}
	}
}

void Texture::Bind(GLenum texture_unit)
{
	//Logger::LogDebug(LOG_POS("Bind"), "%s: Bound texture to %i", m_name.c_str(), texture_unit);
	glActiveTexture(texture_unit);
	glBindTexture(Target_Type(), m_texture);
}

void Texture::Bind()
{
	glBindTexture(Target_Type(), m_texture);
}

