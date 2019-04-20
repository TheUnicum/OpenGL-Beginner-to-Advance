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
	std::vector<std::shared_ptr<Texture>> m_textures;
public:
	FrameBuffer();
	~FrameBuffer();

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetID() const { return m_RendererID; }

	void TextureBind(unsigned int slot = 0) const;

	bool Initialize(int width, int height, int internalFormat = GL_RGB);
	bool Initialize(int width, int height, int internalFormat, int nr_of_colorBuffers);
	bool InitializeDepthMap(int width, int height, bool low_quality = false);
	bool InitializeDepthCubeMap(int width, int height, bool low_quality = false);
	bool InitializeGBuffer(int width, int height);
	bool InitializeGBufferSSAO(int width, int height);
	bool IsComplete() const;
};