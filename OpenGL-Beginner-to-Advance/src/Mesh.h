#pragma once

#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <vector>

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"

#include "Texture.h"
#include "VertexArray.h"
#include "Shader.h"

#include <memory>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;

	Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 texC)
		:Position(pos), Normal(norm), TexCoords(texC) {}

	Vertex(const Vertex& vertex)
		:Position(vertex.Position), Normal(vertex.Normal), TexCoords(vertex.TexCoords) {}
};


class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<std::shared_ptr<Texture>> textures);
	void Draw(std::shared_ptr<Shader> shader, bool compatible_glDrawArrays = false);
	// Utility functions
	inline void vaBind() const { m_va.Bind(); }
	inline void vaUnbind() const { m_va.Unbind(); }
	inline unsigned int GetVertexCount() const { return m_v_count; }
	inline unsigned int GetIndexCount() const { return m_i_count; }
private:
	void setupMesh();

private:
	/* Mesh Data */
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	std::vector<std::shared_ptr<Texture>> msp_Textures;
	// VertexArray & VertexBuffer
	VertexArray m_va;
	std::unique_ptr<VertexBuffer> m_vb;
	std::shared_ptr<IndexBuffer> m_ibo;
	//utility
	unsigned int m_v_count, m_i_count;
	std::string m_textureTypeName[6];
	bool m_ibo_data;
};

// utility overload
std::ostream& operator<<(std::ostream& stream, const Vertex& vertex);
