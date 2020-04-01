#version 330 core
out vec4 FragColor;
  
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;

struct Light {
	vec3 pos;
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
};

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform Material material;
uniform Light light;

void main()
{

	// ambient light
	// float ambientStrength = 0.1; replaced by material
    vec3 ambient = material.ambient * light.ambientColor;
    // diffuse light
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.pos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = (diff * material.diffuse) * light.diffuseColor;
	// specular light
	// float specularStrength = 0.5; replaced by material
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = (material.specular * spec) * light.specularColor;

	vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}