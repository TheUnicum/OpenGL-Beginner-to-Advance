#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "Utility.h"

Shader::Shader(const std::string & filepath)
	: m_FilePath(filepath), m_FilePath_opt1(""), m_FilePath_opt2(""),
	m_RendererID(0)
{
	ShaderProgramSource source;
	ParseShader(filepath, source, ShaderType::NONE);
	m_RendererID = CreateShader(source);
}

Shader::Shader(const std::string & filepath, const std::string & filepath_opt1, const std::string & filepath_opt2)
	: m_FilePath(filepath), m_FilePath_opt1(m_FilePath_opt1), m_FilePath_opt2(m_FilePath_opt2),
	m_RendererID(0)
{
	ShaderProgramSource source;
	ParseShader(filepath, source, ShaderType::VERTEX);
	if (!filepath_opt1.empty())
	{
		if (filepath_opt2.empty())
			ParseShader(filepath_opt1, source, ShaderType::FRAGMENT);
		else
		{	// both file exist	: vs->gs->fs
			ParseShader(filepath_opt1, source, ShaderType::GEOMETRY);
			ParseShader(filepath_opt2, source, ShaderType::FRAGMENT);
		}
	}
	m_RendererID = CreateShader(source);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}

void Shader::ParseShader(const std::string & filepath, ShaderProgramSource & source, ShaderType Shader_type_preselection)
{
	std::ifstream stream(filepath);
	std::stringstream ss[3];
	if (stream.is_open())
	{
		std::string line;
		ShaderType type = Shader_type_preselection;
		while (getline(stream, line))
		{
			if (line.find("#shader") != std::string::npos)
			{
				if (line.find("vertex") != std::string::npos)
					type = ShaderType::VERTEX;
				else if (line.find("geometry") != std::string::npos)
					type = ShaderType::GEOMETRY;
				else if (line.find("fragment") != std::string::npos)
					type = ShaderType::FRAGMENT;
			}
			else
			{
				ss[(int)type] << line << std::endl;
			}
		}
		stream.close();	// close file!
	}
	else
		std::cout << "ERROR: UNABLE TO OPEN SHADERS SOURCE FILE: " << filepath << std::endl;

	source.VertexSource.append(ss[(int)ShaderType::VERTEX].str());
	source.GeometrySource.append(ss[(int)ShaderType::GEOMETRY].str());
	source.FragmentSource.append(ss[(int)ShaderType::FRAGMENT].str());
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	// TODO: Error handling
	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		//char message[length]
		char* message = (char*)alloca(length * sizeof(char));
		GLCall(glGetShaderInfoLog(id, length, &length, message));
		switch (type)
		{
		case GL_VERTEX_SHADER:
			std::cout << "Failed to compile GL_VERTEX_SHADER!" << std::endl;
			break;
		case GL_GEOMETRY_SHADER:
			std::cout << "Failed to compile GL_GEOMETRY_SHADER!" << std::endl;
			break;
		case GL_FRAGMENT_SHADER:
			std::cout << "Failed to compile GL_FRAGMENT_SHADER!" << std::endl;
			break;
		}
		std::cout << message << std::endl;
		GLCall(glDeleteShader(id));
		return 0;
	}
	return id;
}

unsigned int Shader::CreateShader(const ShaderProgramSource& shaderProgramSource)
{
	m_GeometryShaderExist = !shaderProgramSource.GeometrySource.empty();
	GLCall(unsigned int program = glCreateProgram());
	
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, shaderProgramSource.VertexSource);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, shaderProgramSource.FragmentSource);
	unsigned int gs = 0;
	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));

	if (m_GeometryShaderExist)
	{
		unsigned int gs = CompileShader(GL_GEOMETRY_SHADER, shaderProgramSource.GeometrySource);
		GLCall(glAttachShader(program, gs));
	}

	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));
	if (m_GeometryShaderExist)
		GLCall(glDeleteShader(gs));

	return program;
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string & name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1f(const std::string & name, float value)
{
	GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform3f(const std::string & name, float v0, float v1, float v2)
{
	GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}

void Shader::SetUniform4f(const std::string & name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniform2fv(const std::string & name, glm::vec2 & value)
{
	GLCall(glUniform2fv(GetUniformLocation(name), 1, &value[0]));
}

void Shader::SetUniform3fv(const std::string & name, glm::vec3 & value)
{
	GLCall(glUniform3fv(GetUniformLocation(name), 1, &value[0]));
}

void Shader::SetUniform3fv(const std::string & name, int count, glm::vec3 & value)
{
	GLCall(glUniform3fv(GetUniformLocation(name), count, &value[0]));
}

void Shader::SetUniformMat3f(const std::string & name, const glm::mat3 & matrix)
{
	GLCall(glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

void Shader::SetUniformMat4f(const std::string & name, const glm::mat4 & matrix)
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::GetUniformLocation(const std::string & name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1)
		std::cout << "Warning: uniform '" << name << "'doesn't exist!" << std::endl;

	m_UniformLocationCache[name] = location;

	return location;
}
