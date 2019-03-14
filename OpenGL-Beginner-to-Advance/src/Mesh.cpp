#include "Mesh.h"

#include <iostream>

/*
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*>& textures)
{
	this->m_vertices = vertices;
	this->m_indices = indices;
	this->mp_textures = textures;

	// Now that we have all the required data, set the vertex buffers and its attribute pointer.
}
*/

Mesh::Mesh()
{
	setupMesh();
}

void Mesh::setupMesh()
{
	std::cout << "setup Mech ..." << std::endl;
}


// utility overload
std::ostream& operator<<(std::ostream& stream, const Vertex& vertex)
{
	stream << "Pos:(" << vertex.Position.x << ", " << vertex.Position.y << ", " << vertex.Position.z <<
		"), Normal:(" << vertex.Normal.x << ", " << vertex.Normal.y << ", " << vertex.Normal.z <<
		"), TexCoords:(" << vertex.TexCoords.x << ", " << vertex.TexCoords.y << ")";

	return stream;
}