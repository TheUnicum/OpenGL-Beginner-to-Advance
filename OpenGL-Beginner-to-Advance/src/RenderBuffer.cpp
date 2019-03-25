#include "RenderBuffer.h"

RenderBuffer::RenderBuffer()
	: m_RendererID(0)
{
	GLCall(glGenRenderbuffers(1, &m_RendererID));
}

RenderBuffer::~RenderBuffer()
{
	GLCall(glDeleteRenderbuffers(1, &m_RendererID));
}

void RenderBuffer::Bind() const
{
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID));
}

void RenderBuffer::Unbind() const
{
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
}

void RenderBuffer::Initialize(int width, int height, int internalFormat)
{
	m_Width = width;
	m_Height = height;
	m_InternalFormat = internalFormat;
	Bind();
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height));
	Unbind();
}
