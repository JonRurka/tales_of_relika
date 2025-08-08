#include "Cubemap.h"

#include "Resources.h"
#include "Logger.h"
#include "Texture.h"

Cubemap::Shared Create(std::vector<std::string> face_path, bool flip = true)
{
	return std::make_shared<Cubemap>(face_path, flip);
}

Cubemap::Cubemap(std::vector<std::string> face_resources, bool flip)
{
	glGenTextures(1, &m_texture);
	m_initialized = true;
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

	for (unsigned int i = 0; i < face_resources.size(); i++)
	{
		//Resources::Asset asset = Resources::Get_Texture_Asset(face_resources[i]);
		Resources::Load_Texture(face_resources[i], flip);
		std::shared_ptr<Texture> tex = Resources::Get_Texture(face_resources[i]); //static_cast<Texture*>(asset.handle);
		if (tex == nullptr || tex->Data().size() == 0) {
			Logger::LogError(LOG_POS("NEW"), "Failed to load cubemap texture '%s'.", face_resources[i].c_str());
			break;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB, tex->Width(), tex->Height(), 0, GL_RGB, GL_UNSIGNED_BYTE, tex->Data().data()
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

void Cubemap::Bind()
{
	//glActiveTexture(texture_unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
}

void Cubemap::Dispose()
{
	if (!m_initialized)
		return;
	glDeleteTextures(1, &m_texture);
}

