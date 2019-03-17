#include "Mesh.h"

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<std::shared_ptr<Texture>> textures)
{
	this->m_vertices = vertices;
	this->m_indices = indices;
	this->msp_Textures = textures;
	
	this->m_v_count = vertices.size();
	this->m_i_count = indices.size();

	// Now that we have all the required data, set the vertex buffers and its attribute pointer.
	this->setupMesh();

	// Preset the uniform names used int the (mesh).Shader
	this->m_textureTypeName[(int)TextureType::DIFFUSE] = "material.diffuse";
	this->m_textureTypeName[(int)TextureType::SPECULAR] = "material.specular";
	this->m_textureTypeName[(int)TextureType::EMISSION] = "material.emission";
	this->m_textureTypeName[(int)TextureType::NORMAL] = "material.normal";
	this->m_textureTypeName[(int)TextureType::HEIGHT] = "material.height";
}

void Mesh::Draw(std::shared_ptr<Shader> shader)
{
	shader->Bind();

	// msp_Textures[0]->Bind(0);
	// shader->SetUniform1i("material.diffuse1", 0);
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int emissionNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	for (unsigned int i = 0; i < msp_Textures.size(); i++)
	{
		std::string str_indexNr;
		if (this->msp_Textures[i]->GetType() == TextureType::DIFFUSE)
			str_indexNr = std::to_string(diffuseNr++);
		else if (this->msp_Textures[i]->GetType() == TextureType::SPECULAR)
			str_indexNr = std::to_string(specularNr++);
		else if (this->msp_Textures[i]->GetType() == TextureType::EMISSION)
			str_indexNr = std::to_string(emissionNr++);
		else if (this->msp_Textures[i]->GetType() == TextureType::NORMAL)
			str_indexNr = std::to_string(normalNr++);
		else if (this->msp_Textures[i]->GetType() == TextureType::HEIGHT)
			str_indexNr = std::to_string(heightNr++);

		// Now set the sampler to the correct texture unit
		this->msp_Textures[i]->Bind(i);
		shader->SetUniform1i(((m_textureTypeName[(int)this->msp_Textures[i]->GetType()] + str_indexNr).c_str()) , i);
	}
	m_va.Bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));// m_v_count
}

void Mesh::setupMesh()
{
	m_vb = std::make_unique<VertexBuffer>(&m_vertices[0], m_vertices.size() * sizeof(Vertex));
	VertexBufferLayout layout;
	layout.Push<float>(sizeof(Vertex::Position) / sizeof(float));
	layout.Push<float>(sizeof(Vertex::Normal) / sizeof(float));
	layout.Push<float>(sizeof(Vertex::TexCoords) / sizeof(float));
	m_va.AddBuffer(*m_vb, layout);
}

// utility overload for iostream Vertex Data
std::ostream& operator<<(std::ostream& stream, const Vertex& vertex)
{
	stream << "Pos:(" << vertex.Position.x << ", " << vertex.Position.y << ", " << vertex.Position.z <<
		"), Normal:(" << vertex.Normal.x << ", " << vertex.Normal.y << ", " << vertex.Normal.z <<
		"), TexCoords:(" << vertex.TexCoords.x << ", " << vertex.TexCoords.y << ")";
	return stream;
}
