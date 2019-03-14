#pragma once

#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <vector>

#include "Texture.h"

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
	Mesh();
	//Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*>& textures);
private:
	void setupMesh();

private:
	/* Mesh Data */
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	std::vector<Texture> m_textures;
	std::vector<Texture*> mp_textures;
};

// utility overload
std::ostream& operator<<(std::ostream& stream, const Vertex& vertex);
