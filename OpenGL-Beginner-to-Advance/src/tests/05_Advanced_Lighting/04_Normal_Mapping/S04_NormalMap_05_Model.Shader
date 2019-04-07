#shader vertex
#version 330 core

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBN;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat3 u_transInvers_model;


void main()
{
	vec3 T = normalize(vec3(u_model * vec4(aTangent, 0.0f)));
	vec3 B = normalize(vec3(u_model * vec4(aBitangent, 0.0f)));
	vec3 N = normalize(vec3(u_model * vec4(aNormal, 0.0f)));
	TBN = mat3(T, B, N);

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
in mat3 TBN;

struct Material {
	sampler2D diffuse1;
	sampler2D specular1;
	sampler2D emission1;
	sampler2D normal1;
	sampler2D height1;
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
	vec3 normal = texture(material.height1, TexCoords).rgb;
	// trasnsform normal vector to range [-1, +1]
	normal = normalize(normal * 2 - 1.0f);
	vec3 normal_TS = normalize(TBN * normal);

	// Option Active Normal & NormalMap
	vec3 norm;
	if (u_i_NormalMap_opt == 0)
		norm = normalize(Normal);
	else if (u_i_NormalMap_opt == 1)
		norm = normalize(normal);
	else if (u_i_NormalMap_opt == 2)
	{
		if (gl_FragCoord.x < u_i_half_Width)
			norm = normalize(normal);
		else
			norm = normalize(Normal);
	}
	else if (u_i_NormalMap_opt == 3)
	{
		if (gl_FragCoord.x < u_i_half_Width)
			norm = normalize(normal);
		else
			norm = normalize(normal_TS);
	}
	else if (u_i_NormalMap_opt == 4)
		norm = normalize(normal_TS);

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
	float spec = pow(max(dot(NormalActive, halfwayDir), 0.0), material.shininess); // 32.0 //Normal
	//vec3 specular = light.specular * spec * u_b_specular;
	vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords)) * u_b_specular;

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0f);
};
