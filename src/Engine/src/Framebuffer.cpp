#include "Framebuffer.h"

#include "Texture.h"
#include "Graphics.h"
#include "Utilities.h"
#include "Logger.h"

#include <algorithm>

int Framebuffer::m_next_idx{ 0 };

Framebuffer::Framebuffer()
{
	m_id = ++m_next_idx;
}

void Framebuffer::Init()
{
	Refresh();
}

void Framebuffer::Refresh(bool gen_image)
{
	if (m_framebuffer_obj != 0) {
		glDeleteFramebuffers(1, &m_framebuffer_obj);
	}
	//Logger::LogDebug(LOG_POS("Refresh"), "Refresh Framebuffer.");


	glGenFramebuffers(1, &m_framebuffer_obj);
	//glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_obj);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (gen_image) {
		if (m_active_texture.get() != nullptr) {
			m_active_texture->Dispose();
			m_active_texture.reset();
		}

		std::string tex_name = "frame_buff_text_" + std::to_string(m_id);
		m_active_texture = std::make_shared<Texture>(Graphics::Width(), Graphics::Height(), tex_name);
		m_active_texture->Enable_Window_Resize();
		assert(m_active_texture->Initialized());
		Logger::LogDebug(LOG_POS("Refresh"), "Created Framebuffer texture (%i x %i): %i",
			Graphics::Width(), Graphics::Height());
		int i = 0;
	}

	Bind_Texture();
	Init_Depth_Stencil();
	m_initialized = true;
}

void Framebuffer::Bind_Texture()
{
	std::weak_ptr<Framebuffer> this_ptr = shared_from_this();

	if (m_active_texture->m_linked_framebuffers.contains(m_id))
	{
		m_active_texture->m_linked_framebuffers.erase(m_id);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_obj);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_active_texture->Tex(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//m_active_texture = texture;
	m_active_texture->m_linked_framebuffers[m_id] = this_ptr;
	
	//Logger::LogDebug(LOG_POS("Bind_Texture"), "Bind Texture: %i", m_active_texture->m_linked_framebuffers.size());
}

void Framebuffer::Init_Depth_Stencil()
{
	if (m_renderbuffer_obj != 0) {
		glDeleteRenderbuffers(1, &m_renderbuffer_obj);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_obj);

	glGenRenderbuffers(1, &m_renderbuffer_obj);
	glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer_obj);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Graphics::Width(), Graphics::Height());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderbuffer_obj);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

bool Framebuffer::Complete()
{
	assert(m_initialized);

	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_obj);
	bool res = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return res;
}

void Framebuffer::Use(bool active)
{
	assert(m_initialized);

	if (active) {
		//Logger::LogDebug(LOG_POS("Use"), "use frame buffer: %i", (int)m_framebuffer_obj);
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_obj);
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void Framebuffer::Dispose()
{
	if (!m_initialized)
		return;

	if (m_active_texture->m_linked_framebuffers.contains(m_id)){
		m_active_texture->m_linked_framebuffers.erase(m_id);
	}
	m_active_texture->Dispose();
	//m_default_texture = nullptr;

	glDeleteRenderbuffers(1, &m_renderbuffer_obj);
	glDeleteFramebuffers(1, &m_framebuffer_obj);
}
