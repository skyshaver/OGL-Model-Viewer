#version 330 core
out vec4 FragColor;
 
struct Light {
	vec3 pos;
	vec3 direction;
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;

	float constant;
	float linear;
	float quadratic;
};

// struct Material {
// 	vec3 ambient;
// 	vec3 diffuse;
// 	vec3 specular;
// 	float shininess;
// };

struct Material {
	sampler2D diffuse;
	sampler2D specular;
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
	vec3 lightDir = normalize(-light.pos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuseColor * diff * texture(material.diffuse, TexCoords).rgb;
	
	// specular light
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specularColor * spec * texture(material.diffuse, TexCoords).rgb;

	// calculate attenuation variables
	float distance = length(light.pos - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + 
    		    light.quadratic * (distance * distance));   

	ambient *= attenuation;
	diffuse *= attenuation;
	specular*= attenuation;

	vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}