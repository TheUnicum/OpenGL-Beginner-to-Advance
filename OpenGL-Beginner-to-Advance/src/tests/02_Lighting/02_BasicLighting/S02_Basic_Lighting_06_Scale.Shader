#shader vertex
#version 330 core

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat3 u_transInvers_model;


void main()
{
	gl_Position = u_mvp * aPos;
	FragPos = vec3(u_model * aPos);
	//Normal = mat3(transpose(inverse(u_model))) * aNormal; // u_transInvers_model * aNormal;
	Normal = u_transInvers_model * aNormal;
};

#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

//uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

uniform int u_b_ambient;
uniform int u_b_diffuse;
uniform int u_b_specular;

void main()
{
	// ambient
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor * u_b_ambient;

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = diff * lightColor * u_b_diffuse;

	// specular
	float specularStrength = 0.5f;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	vec3 specular = specularStrength * spec * lightColor * u_b_specular;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	FragColor = vec4(result, 1.0f);
};

