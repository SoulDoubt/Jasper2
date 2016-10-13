#include "Common.h"
#include "GBuffer.h"
#include <GL/glew.h>

GBuffer::GBuffer(int w, int h):m_height(h), m_width(w)
{
}

GBuffer::~GBuffer()
{
}

constexpr int TexCount() {
	return (int)GBuffer::TextureType::NUM_TEXTURES;
}

void GBuffer::Initialize()
{
	glGenFramebuffers(TexCount(), &m_fboID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fboID);

	glGenTextures(TexCount(), m_textureIDs);
	glGenTextures(1, &m_depthTextureID);

	// position, diffuse, normal, texcoord
	for (unsigned i = 0; i < TexCount(); i++) {
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_width, m_height, 0, GL_RGB, GL_FLOAT, nullptr);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textureIDs[i], 0);
	}
	// depth
	glBindTexture(GL_TEXTURE_2D, m_depthTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTextureID, 0);

	GLenum drawBuffers[TexCount()];
	for (int i = 0; i < TexCount(); i++) {
		drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;

	}
	glDrawBuffers(TexCount(), drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("Error Creating Framebuffer");
	}
	else {
		printf("Framebuffer created succussfully.");
	}

	// restore everything...
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void GBuffer::Destroy()
{
}
