#shader vertex
#version 330 core

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat3 u_transInvers_model;


void main()
{
	gl_Position = u_mvp * aPos;
	FragPos = vec3(u_model * aPos);
	Normal = u_transInvers_model * aNormal;
};

#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform vec3 u_viewPos;
uniform Material material;
uniform Light light;

uniform int u_b_ambient;
uniform int u_b_diffuse;
uniform int u_b_specular;

void main()
{
	// ambient
	vec3 ambient = light.ambient * material.ambient * u_b_ambient;

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = light.diffuse * (diff * material.diffuse) * u_b_diffuse;

	// specular
	vec3 viewDir = normalize(u_viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	vec3 specular = light.specular * (spec * material.specular) * u_b_specular;

	vec3 result = (ambient + diffuse + specular);
	FragColor = vec4(result, 1.0f);
};

