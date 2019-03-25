#pragma once

#include "Utility.h"

class RenderBuffer
{
private:
	unsigned int m_RendererID;
	int m_Width, m_Height, m_InternalFormat;
public:
	RenderBuffer();
	~RenderBuffer();

	void Bind() const;
	void Unbind() const;

	void Initialize(int width, int height,
		int internalFormat = GL_DEPTH24_STENCIL8);

	inline unsigned int GetID() const { return m_RendererID; }
	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	inline int GetIntFormat() const { return m_InternalFormat; }
};
