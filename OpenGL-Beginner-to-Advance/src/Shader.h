#pragma once

#include <string>
#include <unordered_map>

#include "glm/glm.hpp"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string GeometrySource;
	std::string FragmentSource;
};

enum class ShaderType
{
	NONE = -1, VERTEX = 0, GEOMETRY = 1, FRAGMENT = 2
};

class Shader
{
private:
	std::string m_FilePath, m_FilePath_opt1, m_FilePath_opt2;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;
	bool m_GeometryShaderExist;
public:
	Shader(const std::string& filepath);
	// vertexShader + fragmentShader
	// vertexShader + geometryShader + fragmentShader
	Shader(const std::string& filepath, const std::string& filepath_opt1, const std::string& filepath_opt2 = "");

	~Shader();

	void Bind() const;
	void Unbind() const;

	// Set uniforms
	void SetUniform1i(const std::string& name, int value);

	void SetUniform1f(const std::string& name, float value);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

	void SetUniform2fv(const std::string& name, glm::vec2& value);
	void SetUniform3fv(const std::string& name, glm::vec3& value);
	void SetUniform3fv(const std::string& name, int count, glm::vec3& value);

	void SetUniformMat3f(const std::string& name, const glm::mat3& matrix);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
	inline unsigned int GetID() const { return m_RendererID; }
private:
	void ParseShader(const std::string& filepath, ShaderProgramSource & source, ShaderType Shader_type_preselection = ShaderType::NONE);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	//unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	unsigned int CreateShader(const ShaderProgramSource& shaderProgramSource);
	int GetUniformLocation(const std::string& name);
};
