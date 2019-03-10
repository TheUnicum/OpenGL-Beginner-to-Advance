#shader vertex
#version 330 core

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 LightingColor; // resulting color from lighting calculations

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat4 u_view;

uniform int u_b_ambient;
uniform int u_b_diffuse;
uniform int u_b_specular;

void main()
{
	gl_Position = u_mvp * aPos;

	// gouraud shading
	// ------------------------
	vec3 Position = vec3(u_model * aPos);
	vec3 Normal = mat3(transpose(inverse(u_model))) * aNormal;

	// ambient
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor * u_b_ambient;

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - Position);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = diff * lightColor * u_b_diffuse;

	// specular
	float specularStrength = 1.0; // this is set higher to better show the effect of Gouraud shading 
	vec3 viewDir = normalize(viewPos - Position);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	vec3 specular = specularStrength * spec * lightColor * u_b_specular;

	LightingColor = ambient + diffuse + specular;

};

#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 LightingColor;

uniform vec3 objectColor;

void main()
{
	FragColor = vec4(LightingColor * objectColor, 1.0);
}