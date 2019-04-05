#shader vertex
#version 330 core

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat3 u_transInvers_model;


void main()
{
	FragPos = vec3(u_model * aPos);
	Normal = u_transInvers_model * aNormal;
	TexCoords = aTexCoords;

	gl_Position = u_mvp * aPos;
};

#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material {
	sampler2D diffuse1;
	sampler2D specular1;
	sampler2D emission1;
	sampler2D normal1;
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

uniform int u_i_NormalMap_opt;
uniform int u_i_half_Width;
uniform bool u_b_NorMap_diffuse;
uniform bool u_b_NorMap_specular;

void main()
{
	// obtain normal map in range [0, 1]
	vec3 normal = texture(material.normal1, TexCoords).rgb;
	// trasnsform normal vector to range [-1, +1]
	normal = normalize(normal * 2 - 1.0f);

	// Option Active Normal & NormalMap
	vec3 norm;
	if (u_i_NormalMap_opt == 0)
		norm = normalize(Normal);
	else if (u_i_NormalMap_opt == 1)
		norm = normalize(normal);
	else
	{
		if (gl_FragCoord.x < u_i_half_Width)
			norm = normalize(normal);
		else
			norm = normalize(Normal);
	}
	//----------------------------

	// ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse1, TexCoords)) * u_b_ambient;

	// diffuse
	vec3 NormalActive;
	if (u_b_NorMap_diffuse)
		NormalActive = norm;
	else
		NormalActive = normalize(Normal);

	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(NormalActive, lightDir), 0.0f);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse1, TexCoords)) * u_b_diffuse;

	// specular
	if (u_b_NorMap_specular)
		NormalActive = norm;
	else
		NormalActive = normalize(Normal);

	vec3 viewDir = normalize(u_viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, NormalActive);  // Normal
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(NormalActive, halfwayDir), 0.0), 32.0); //Normal
	vec3 specular = light.specular * spec * u_b_specular;


	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0f);
};
