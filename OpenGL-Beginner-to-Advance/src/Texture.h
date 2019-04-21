#pragma once

#include <string>
#include <vector>

#include "Utility.h"

#include "glm/glm.hpp"

enum class TextureType
{
	NONE = -1, DIFFUSE = 0, SPECULAR = 1, EMISSION = 2, NORMAL = 3, HEIGHT = 4, AMBIENT = 5,
	ALBEDO = 6, METALLIC = 7, ROUGHNESS = 8, AO = 9,
	MAX_COUNT = 12 // useful for std::string m_textureTypeName[(int)TextureType::MAX_COUNT]; on Mesh.h
};

class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	std::string m_Directory;
	std::vector<std::string> m_FilePaths;
	unsigned char* m_LocalBuffer;
	float* m_fp_LocalBuffer;
	int m_Width, m_Height, m_BPP;
	TextureType m_type;
	int m_target;
	bool m_flip_vertically;
public:
	// Default empty constructor
	Texture();

	// Simple constructor for manual use.
	Texture(const std::string& path,
		int min_filter = GL_LINEAR,
		int mag_filter = GL_LINEAR,
		int wrap_s = GL_CLAMP_TO_EDGE,
		int wrap_t = GL_CLAMP_TO_EDGE,
		bool flip_vertically = true);

	// Intermediate constructor for MESH/MODEL files.
	Texture(const std::string& path,
		TextureType textureType,
		bool flip_vertically = true,
		int min_filter = GL_LINEAR_MIPMAP_LINEAR,
		int mag_filter = GL_LINEAR,
		int wrap_s = GL_REPEAT,		// important for loading MODEL: Texture MUST NOT BE CLAMPED TO EDGE!!!!!!
		int wrap_t = GL_REPEAT);

	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	// Initilalize for custom/framebuffer textures. <could be useful change name in Initialize2D(...)>
	void Initialize(int width, int height,
		int internalFormat = GL_RGB,
		int dataFormat = GL_RGB,
		int min_filter = GL_LINEAR,
		int mag_filter = GL_LINEAR,
		int wrap_s = GL_REPEAT,	// default value
		int wrap_t = GL_REPEAT,
		glm::vec4 borderColor = glm::vec4(-1.0f));

	// Initialize for custom/framebuffer CUBO textures.
	void InitializeCube(int width, int height,
		int internalFormat = GL_DEPTH_COMPONENT,
		int dataFormat = GL_DEPTH_COMPONENT,
		int min_filter = GL_LINEAR,
		int mag_filter = GL_LINEAR,
		int wrap_s = GL_REPEAT,
		int wrap_t = GL_REPEAT,
		int wrap_r = GL_REPEAT,
		glm::vec4 borderColor = glm::vec4(-1.0f));

	void LoadCubemap(std::vector<std::string>& faces, std::string directory);
	void LoadHdr(const std::string& path);

	inline unsigned int GetID() const { return m_RendererID; }
	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	inline int GetBPP() const { return m_BPP; }
	inline TextureType GetType() const { return m_type; }
	inline int GetTarget() const { return m_target; }

	inline std::string GetPath() const { return m_FilePath; }
	inline std::string GetDirectory() const { return m_Directory; }
	inline std::vector<std::string> GetPaths() const { return m_FilePaths; }
};