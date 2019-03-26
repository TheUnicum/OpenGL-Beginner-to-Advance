#include "Texture.h"

#include <iostream>
#include "stb_image/stb_image.h"

Texture::Texture()
	: m_RendererID(0), m_FilePath(""), m_LocalBuffer(nullptr),
	m_Width(0), m_Height(0), m_BPP(0),
	m_type(TextureType::NONE),
	m_target(GL_TEXTURE_2D)
{
	GLCall(glGenTextures(1, &m_RendererID));
}

Texture::Texture(const std::string & path, int min_filter, int mag_filter, int wrap_s, int wrap_t)
	: m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr),
	m_Width(0), m_Height(0), m_BPP(0),
	m_type(TextureType::DIFFUSE),
	m_target(GL_TEXTURE_2D)
{
	stbi_set_flip_vertically_on_load(true);
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height,&m_BPP, 4);

	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter)); // GL_LINEAR
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter)); // GL_LINEAR
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s)); // GL_CLAMP_TO_EDGE
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t)); // GL_CLAMP_TO_EDGE


	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
	GLCall(glGenerateMipmap(GL_TEXTURE_2D));

	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);
	else
		std::cout << "ERROR::Failed to load Texture!" << std::endl;
}

Texture::Texture(const std::string & path, TextureType textureType, int min_filter, int mag_filter, int wrap_s, int wrap_t)
	: Texture(path, min_filter, mag_filter, wrap_s, wrap_t) { m_type = textureType; }

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot /*= 0*/) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(m_target, m_RendererID));	// old target GL_TEXTURE_2D
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(m_target, 0));
}

void Texture::Initialize(int width, int height, int internalFormat, int dataFormat, int min_filter, int mag_filter, int wrap_s, int wrap_t)
{
	m_LocalBuffer = nullptr;
	m_Width = width;
	m_Height = height;
	m_BPP = 0;
	m_target = GL_TEXTURE_2D;

	Bind();
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, nullptr));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter)); // GL_LINEAR
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter)); // GL_LINEAR
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s)); // GL_REPEAT
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t)); // GL_REPEAT
	Unbind();
}

void Texture::LoadCubemap(std::vector<std::string>& faces, std::string directory)
{
	m_FilePaths = faces;
	m_Directory = directory;
	m_LocalBuffer = nullptr;
	m_target = GL_TEXTURE_CUBE_MAP;

	Bind();

	bool width_and_height_stored = false;
	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		stbi_set_flip_vertically_on_load(false);
		m_LocalBuffer = stbi_load((directory + faces[i]).c_str(), &width, &height, &nrChannels, 4);
		if (m_LocalBuffer)
		{
			GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
			
			if (!width_and_height_stored)
			{
				m_Width = width;
				m_Height = height;
				m_BPP = nrChannels;
			}
		}
		else
		{
			std::cout << "ERROR::Cubemap texture failed to load path: " << faces[i] << std::endl;
		}
		stbi_image_free(m_LocalBuffer);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	Unbind();
}
