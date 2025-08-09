#pragma once

#include "opengl.h"

#include <string>
#include <memory>
#include <assert.h>

class Texture;

class Framebuffer : public std::enable_shared_from_this<Framebuffer>
{
public:
	typedef std::shared_ptr<Framebuffer> Shared;
	typedef std::weak_ptr<Framebuffer> Weak;

	Framebuffer();
	~Framebuffer() {
		Dispose();
	}

	void Init();

	void Init_Depth_Stencil();

	void Refresh(bool gen_image = true);

	void Bind_Texture();

	bool Complete();

	void Use(bool active);

	void Dispose();

	std::shared_ptr<Texture> Active_Texture() 
	{ 
		assert(m_initialized);
		return m_active_texture; 
	}

private:
	GLuint m_framebuffer_obj{ 0 };
	GLuint m_renderbuffer_obj{ 0 };
	//std::shared_ptr<Texture> m_default_texture{nullptr};
	std::shared_ptr<Texture> m_active_texture;
	bool m_initialized{ false };
	int m_id{ 0 };

	static int m_next_idx;

	inline static const std::string LOG_LOC{ "FRAME_BUFFER" };
};