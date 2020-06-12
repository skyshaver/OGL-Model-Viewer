#include "PointLight.h"
#include "Shader.h"
#include <fmt/format.h>


PointLight::PointLight() :
	pos(glm::vec3{ -3.f, 1.f, 0.0f }),
	ambient(glm::vec3{ .25f }), diffuse(glm::vec3{ 0.8f }), specular(glm::vec3{ 1.f }),
	constant(1.f), linear(0.09f), quadratic(0.012f)
{

}

PointLight::PointLight(glm::vec3 pos, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic) :
	pos(pos), ambient(ambient), diffuse(diffuse),
	specular(specular), constant(constant), linear(linear), quadratic(quadratic)
{

}

void PointLight::updateLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) 
{
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
}

void PointLight::setShaderUniforms(Shader& shader, const std::string& uniformName) const
{
	shader.setVec3(fmt::format(uniformName + ".position"), pos);
	shader.setVec3(fmt::format(uniformName + ".ambient"), this->ambient);
	shader.setVec3(fmt::format(uniformName + ".diffuse"), this->diffuse);
	shader.setVec3(fmt::format(uniformName + ".specular"), this->specular);
	shader.setFloat(fmt::format(uniformName + ".constant"), this->constant);
	shader.setFloat(fmt::format(uniformName + ".linear"), this->linear);
	shader.setFloat(fmt::format(uniformName + ".quadratic"), this->quadratic);
}
