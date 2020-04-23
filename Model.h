#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



#include <stb_image.h>

#include "Shader.h"
#include "Mesh.h"

class Model
{
public:
	
	Model(const char* path) { loadModel(path); }
	void Draw(Shader shader) const;


private:

	std::vector<Mesh> meshes;
	std::vector<Texture> textures_loaded;
	std::string directory;

	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);
};

