#pragma once

// assimp lib import <assimp-4.1.0>
// Important: after generate lib with cMake remember to add include\assimp\config.h
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <memory>

#include <string>

#include "Texture.h"
#include "Mesh.h"
#include "Shader.h"

#include <iostream>


class Model
{
public:
	/* Functions */
	Model(const std::string& path, bool calcTangentSpace = false) { loadModel(path, calcTangentSpace); }
	// draws the model, and thus all its meshes
	void Draw(std::shared_ptr<Shader> shader, bool compatible_glDrawArrays = false);
	// draws the model Nr Times, and thus all its meshes
	void Draw(std::shared_ptr<Shader> shader, unsigned int instance_count, bool compatible_glDrawArrays = false);

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout, unsigned int attrP_offset);
private:
	/* Funcitons */
	// loads model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(const std::string& path, bool calcTangentSpace);

	// process a node in a recursive fashion. Processes each individual mesh located at the node 
	// and repeats this  process on its children nodes (if any)
	void processNode(aiNode* node, const aiScene* scene);
	std::shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType textureType);

private:
	/* Model Data */
	std::vector<std::shared_ptr<Texture>> msp_texture_loaded;	// store all the texture loaded so far, optimization to make 
																// sure texture aren't loaded more than once.
	std::vector<std::shared_ptr<Mesh>> m_meshes;
	std::string m_directory;
};