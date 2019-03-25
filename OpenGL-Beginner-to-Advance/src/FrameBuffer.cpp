#include "FrameBuffer.h"

FrameBuffer::FrameBuffer()
	: m_RendererID(0)
{
	GLCall(glGenFramebuffers(1, &m_RendererID));
}

FrameBuffer::~FrameBuffer()
{
	GLCall(glDeleteFramebuffers(1, &m_RendererID));
}

void FrameBuffer::Bind() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
}

void FrameBuffer::Unbind() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::TextureBind(unsigned int slot /*= 0*/) const
{
	m_texture.Bind(slot);
}

// Initialize FBO standard:
// 1- Attachment texture 
// 2- depth/stencil RenderBuffer
bool FrameBuffer::Initialize(int width, int height)
{
	// - We have to attach at least one buffer(color, depth or stencil buffer).
	// - There should be at least one color attachment.
	// - All attachments should be complete as well(reserved memory).
	// - Each buffer should have the same number of samples.

	// 1- framebuffer configuration 
	Bind();
	// 2- create a color attachment texture
	m_texture.Initialize(width, height);
	// 3- attach it to currently bound framebuffer object
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture.GetID(), 0));
	// 4- create a render buffer object for depth and stencil attachment ( we won't be sampling these)
	m_rbo.Initialize(width, height);
	// 5- attach it to currently bound framebuffer object
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo.GetID()));
	// 6- Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	bool complete = IsComplete();
	// 7- Unbind FrameBuffer and reset to default
	Unbind();

	return complete;
}

bool FrameBuffer::IsComplete() const
{
	return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}
