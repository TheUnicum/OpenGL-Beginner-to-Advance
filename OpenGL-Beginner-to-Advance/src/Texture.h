#pragma once

#include <string>
#include "Utility.h"

class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
public:
	Texture(const std::string& path,
		int min_filter = GL_LINEAR,
		int mag_filter = GL_LINEAR,
		int wrap_s = GL_CLAMP_TO_EDGE,
		int wrap_t = GL_CLAMP_TO_EDGE);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};