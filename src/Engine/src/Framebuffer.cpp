#include "Framebuffer.h"

#include "Texture.h"
#include "Graphics.h"
#include "Utilities.h"

#include <algorithm>

Framebuffer::Framebuffer()
{
	Refresh();
}

void Framebuffer::Refresh(bool gen_image)
{
	if (m_framebuffer_obj != 0) {
		glDeleteFramebuffers(1, &m_framebuffer_obj);
	}

	glGenFramebuffers(1, &m_framebuffer_obj);
	//glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_obj);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (gen_image) {
		m_default_texture = new Texture(Graphics::Width(), Graphics::Height());
		m_active_texture = m_default_texture;
	}

	Bind_Texture(m_active_texture);
	Init_Depth_Stencil();
}

void Framebuffer::Bind_Texture(Texture* texture)
{
	if (m_active_texture != nullptr) {
		Remove_If_Found(m_active_texture->m_linked_framebuffers, this);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_obj);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_active_texture->Tex(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_active_texture = texture;
	m_active_texture->m_linked_framebuffers.push_back(this);
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
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_obj);
	bool res = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return res;
}

void Framebuffer::Use(bool active)
{
	if (active) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_obj);
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
