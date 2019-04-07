#include "Model.h"


void Model::Draw(std::shared_ptr<Shader> shader, bool compatible_glDrawArrays)
{
	for (unsigned int i = 0; i < m_meshes.size(); i++)
		m_meshes[i]->Draw(shader, compatible_glDrawArrays);
}

void Model::Draw(std::shared_ptr<Shader> shader, unsigned int instance_count, bool compatible_glDrawArrays)
{
	for (unsigned int i = 0; i < m_meshes.size(); i++)
		m_meshes[i]->Draw(shader, instance_count, compatible_glDrawArrays);
}

void Model::loadModel(const std::string & path, bool calcTangentSpace)
{
	// read file via ASSIMP
	// aiProcess_Triangulate       : transform all the model's primitive shapes to triangles
	// aiProcess_FlipUVs           : flips the texture coordinates on the y - axis
	// aiProcess_CalcTangentSpace  : Calculates the tangents and bitangents for the imported meshes.
	// for more https://learnopengl.com/Model-Loading/Model
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	// retrive the directory path of the filepath
	m_directory = path.substr(0, path.find_last_of('/'));
	
	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);

	std::cout << "----------> End load model" << std::endl;
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene.
		// the scene contains all the data, node is jsut to keep stuff organuzed (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(processMesh(mesh, scene));
	}
	// after we processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

std::shared_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// data to fill
	std::vector<VertexTB> vertices; //std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<std::shared_ptr<Texture>> sp_textures;

	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		//Vertex vertex(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec2(1.0f));
		glm::vec3 v_pos, v_norm, v_tang, v_bitang;
		glm::vec2 v_texCoord;
		
		glm::vec3 vector;	// we declare a placeholder vector since assimp its own vector class that doesn't
							// directly convert to glm's vec3 class so we transfer the data to this palceholder glm::vec3 first
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		//vertex.Position = vector;
		v_pos = vector;
		// normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		//vertex.Normal = vector;
		v_norm = vector;
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contains texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			//vertex.TexCoords = vec;
			v_texCoord = vec;
		}
		else
		{
			//vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			v_texCoord = glm::vec2(0.0f, 0.0f);
		}
		// tangent
		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		v_tang = vector;
		// bitangent
		vector.x = mesh->mBitangents[i].x;
		vector.y = mesh->mBitangents[i].y;
		vector.z = mesh->mBitangents[i].z;
		v_bitang = vector;

		//vertices.emplace_back(v_pos, v_norm, v_texCoord);
		vertices.emplace_back(v_pos, v_norm, v_texCoord, v_tang, v_bitang);
		//vertices.push_back(vertex);
	}

	// now walk through each mesh's faces (a face is a mesh its triangle)
	// and retrive the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrive all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be names
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
	
	// 1. diffuse maps
	std::vector<std::shared_ptr<Texture>> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::DIFFUSE);
	sp_textures.insert(sp_textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	std::vector<std::shared_ptr<Texture>> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::SPECULAR);
	sp_textures.insert(sp_textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. emission maps
	std::vector<std::shared_ptr<Texture>> emissionMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, TextureType::EMISSION);
	sp_textures.insert(sp_textures.end(), emissionMaps.begin(), emissionMaps.end());
	// 4. normal maps
	std::vector<std::shared_ptr<Texture>> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, TextureType::NORMAL);
	sp_textures.insert(sp_textures.end(), normalMaps.begin(), normalMaps.end());
	// 5. height maps
	std::vector<std::shared_ptr<Texture>> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, TextureType::HEIGHT);
	sp_textures.insert(sp_textures.end(), heightMaps.begin(), heightMaps.end());
	// 6. ambient map
	std::vector<std::shared_ptr<Texture>> ambientMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, TextureType::AMBIENT);
	sp_textures.insert(sp_textures.end(), ambientMaps.begin(), ambientMaps.end());

	return std::make_shared<Mesh>(vertices, indices, sp_textures);
}

std::vector<std::shared_ptr<Texture>> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType textureType)
{

	std::vector<std::shared_ptr<Texture>> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		std::string filePath = m_directory + '/' + str.C_Str();


		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;

		for (unsigned int j = 0; j < msp_texture_loaded.size(); j++)
		{
			if (std::strcmp(msp_texture_loaded[j]->GetPath().c_str(), filePath.c_str()) == 0)
			{
				textures.push_back(msp_texture_loaded[j]);
				skip = true; // a texture with the same filePath has already been loaded, continue to next one. (optimization)
				std::cout << "OPTIMIZED" << filePath << std::endl;
				break;
			}
		}
		if (!skip)
		{	// if texture hasn't been loaded already, load it
			std::shared_ptr<Texture> sp_mTexture = std::make_shared<Texture>(filePath, textureType);
			textures.push_back(sp_mTexture);
			msp_texture_loaded.push_back(sp_mTexture);  // store it as texture loaded for entire model, 
														// to ensure we won't unnecesery load duplicated tectures.
		}
	}

	return textures;
}

void Model::AddBuffer(const VertexBuffer & vb, const VertexBufferLayout & layout, unsigned int attrP_offset)
{
	for (unsigned int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i]->AddBuffer(vb, layout, attrP_offset);

		/*
		m_meshes[i]->m_va.AddBuffer(vb, layout, AttrP_offset);

		// to insert in layout information
		GLCall(glVertexAttribDivisor(3, 1));
		GLCall(glVertexAttribDivisor(4, 1));
		GLCall(glVertexAttribDivisor(5, 1));
		GLCall(glVertexAttribDivisor(6, 1));
		*/
	}
}

