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

Texture::Texture(const std::string & path, int min_filter, int mag_filter, int wrap_s, int wrap_t, bool flip_vertically)
	: m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr),
	m_Width(0), m_Height(0), m_BPP(0),
	m_type(TextureType::DIFFUSE),
	m_target(GL_TEXTURE_2D),
	m_flip_vertically(flip_vertically)
{
	stbi_set_flip_vertically_on_load(flip_vertically);// (true);
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
		std::cout << "ERROR::Failed to load Texture! :" << path << std::endl;
}

Texture::Texture(const std::string & path, TextureType textureType, bool flip_vertically, int min_filter, int mag_filter, int wrap_s, int wrap_t)
	: Texture(path, min_filter, mag_filter, wrap_s, wrap_t, flip_vertically) { m_type = textureType; }

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

void Texture::Initialize(int width, int height, int internalFormat, int dataFormat, int min_filter, int mag_filter, int wrap_s, int wrap_t, glm::vec4 borderColor)
{
	m_LocalBuffer = nullptr;
	m_Width = width;
	m_Height = height;
	m_BPP = 0;
	m_target = GL_TEXTURE_2D;

	Bind();
	// Implementation for HDR (High Dynamic Range) Floating Point framebuffer
	if ((internalFormat == GL_RGBA16F) || (internalFormat == GL_RGBA32F) || (internalFormat == GL_RG16F) ||
		(internalFormat == GL_DEPTH_COMPONENT) || (internalFormat == GL_RED))
	{
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_FLOAT, nullptr));
	}
	else
	{
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, nullptr));
	}

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter)); // GL_LINEAR
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter)); // GL_LINEAR
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s)); // GL_REPEAT
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t)); // GL_REPEAT

	// Optimization for Shadow Mapping
	if (borderColor != glm::vec4(-1.0f))
	{
		GLCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &borderColor.x));
	}

	Unbind();
}

void Texture::InitializeCube(int width, int height,
	int internalFormat, int dataFormat,
	int min_filter, int mag_filter,
	int wrap_s, int wrap_t, int wrap_r,
	glm::vec4 borderColor)
{
	m_LocalBuffer = nullptr;
	m_Width = width;
	m_Height = height;
	m_BPP = 0;
	m_target = GL_TEXTURE_CUBE_MAP;

	Bind();

	for (unsigned int i = 0; i < 6; i++)
	{
		// Implementation for HDR (High Dynamic Range) Floating Point framebuffer
		int data_type;
		if ((internalFormat == GL_RGBA16F) || (internalFormat == GL_RGBA32F) || (internalFormat == GL_DEPTH_COMPONENT) || (internalFormat == GL_RED))
			data_type = GL_FLOAT;
		else
			data_type = GL_UNSIGNED_BYTE;

		GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, m_Width, m_Height, 0, dataFormat, data_type, NULL));
	}

	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, min_filter));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, mag_filter));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap_s));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap_t));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap_t));

	// Optimization for Shadow Mapping
	if (borderColor != glm::vec4(-1.0f))
	{
		GLCall(glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, &borderColor.x));
	}

	// Automatically generate MipMap if GL_TEXTURE_MIN_FILTER is setted to GL_LINEAR_MIPMAP_LINEAR !
	if (min_filter == GL_LINEAR_MIPMAP_LINEAR)
		GLCall(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));

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

void Texture::LoadHdr(const std::string& path)
{
	m_FilePath = path;
	m_fp_LocalBuffer = nullptr;
	m_target = GL_TEXTURE_2D;

	stbi_set_flip_vertically_on_load(true);
	m_fp_LocalBuffer = stbi_loadf(path.c_str(), &m_Width, &m_Height, &m_BPP, 0);

	if (m_fp_LocalBuffer)
	{
		Bind();
		// note how we specify the texture's data value to be float
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, m_fp_LocalBuffer)); 

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		stbi_image_free(m_fp_LocalBuffer);

		Unbind();
	}
	else
	{
		std::cout << "ERROR::Failed to load HDR Texture! :" << path << std::endl;
	}

}
