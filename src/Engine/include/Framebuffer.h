#pragma once

#include "opengl.h"

#include <string>

class Texture;

class Framebuffer {
public:

	Framebuffer();

	void Init_Depth_Stencil();

	void Refresh(bool gen_image = true);

	void Bind_Texture(Texture* texture);

	bool Complete();

	void Use(bool active);

	void Dispose();

	Texture* Active_Texture() { return m_active_texture; }

private:
	GLuint m_framebuffer_obj{ 0 };
	GLuint m_renderbuffer_obj{ 0 };
	Texture* m_default_texture{nullptr};
	Texture* m_active_texture{ nullptr };

	inline static const std::string LOG_LOC{ "FRAME_BUFFER" };
};