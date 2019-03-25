#pragma once

#include <string>
#include "Utility.h"

enum class TextureType
{
	NONE = -1, DIFFUSE = 0, SPECULAR = 1, EMISSION = 2, NORMAL = 3, HEIGHT = 4, AMBIENT = 5
};

class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
	TextureType m_type;
public:
	Texture();

	Texture(const std::string& path,
		int min_filter = GL_LINEAR,
		int mag_filter = GL_LINEAR,
		int wrap_s = GL_CLAMP_TO_EDGE,
		int wrap_t = GL_CLAMP_TO_EDGE);

	Texture(const std::string& path,
		TextureType textureType,
		int min_filter = GL_LINEAR_MIPMAP_LINEAR,
		int mag_filter = GL_LINEAR,
		int wrap_s = GL_REPEAT,		// important for loading MODEL: Texture MUST NOT BE CLAMPED TO EDGE!!!!!!
		int wrap_t = GL_REPEAT);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	void Initialize(int width, int height,
		int internalFormat = GL_RGB,
		int dataFormat = GL_RGB,
		int min_filter = GL_LINEAR,
		int mag_filter = GL_LINEAR,
		int wrap_s = GL_REPEAT,	// default value
		int wrap_t = GL_REPEAT);

	inline unsigned int GetID() const { return m_RendererID; }
	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	inline int GetBPP() const { return m_BPP; }
	inline TextureType GetType() const { return m_type; }
	inline std::string GetPath() const { return m_FilePath; }
};