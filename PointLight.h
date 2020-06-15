#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

class Shader;

class PointLight
{
public:

	PointLight();
	PointLight(glm::vec3 pos);
	PointLight(glm::vec3 pos, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic);

	void setShaderUniforms(Shader& shader, const std::string& name) const;

	void updateLight(glm::vec3 ambient,glm::vec3 diffuse, glm::vec3 specular);


	glm::vec3 pos;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float constant;
	float linear;
	float quadratic;
};
