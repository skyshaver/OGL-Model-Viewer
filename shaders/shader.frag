#version 330 core
out vec4 FragColor;
 
struct Light {
	vec3 pos;
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
};

// struct Material {
// 	vec3 ambient;
// 	vec3 diffuse;
// 	vec3 specular;
// 	float shininess;
// };

struct Material {
	sampler2D diffuse;
	vec3 specular;
	float shininess;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform Material material;
uniform vec3 viewPos;
uniform Light light;

void main()
{

	// ambient light
    vec3 ambient = light.ambientColor * texture(material.diffuse, TexCoords).rgb;

    // diffuse light
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.pos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuseColor * texture(material.diffuse, TexCoords).rgb;;
	
	// specular light
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specularColor * (material.specular * spec);

	vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}