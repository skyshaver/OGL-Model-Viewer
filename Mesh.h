#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "Shader.h"

struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};

// ordering of members is important here as we're using offsetof to set string in gl functions, order should be Postion, Normal, TexCoords
struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};


class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures);
	void Draw(Shader shader) const;
private:
	unsigned int VBO, VAO, EBO;
	void setUpMesh();
};

