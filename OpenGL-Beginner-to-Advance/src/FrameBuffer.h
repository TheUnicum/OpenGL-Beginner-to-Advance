#pragma once

#include "Utility.h"

#include "Texture.h"
#include "RenderBuffer.h"

class FrameBuffer
{
private:
	unsigned int m_RendererID;
	int m_Width, m_Height;
	Texture m_texture;
	RenderBuffer m_rbo;

public:
	FrameBuffer();
	~FrameBuffer();

	void Bind() const;
	void Unbind() const;

	void TextureBind(unsigned int slot = 0) const;

	bool Initialize(int width, int height);
	bool InitializeDepthMap(int width, int height);
	bool IsComplete() const;
};