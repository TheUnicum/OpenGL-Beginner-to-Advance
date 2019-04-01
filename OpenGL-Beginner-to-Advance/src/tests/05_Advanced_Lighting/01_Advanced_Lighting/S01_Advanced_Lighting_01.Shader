#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} vs_out;

uniform mat4 u_mvp;

void main()
{
	vs_out.FragPos = aPos;
	vs_out.Normal = aNormal;
	vs_out.TexCoords = aTexCoords;
	gl_Position = u_mvp * vec4(aPos, 1.0f);
};


#shader fragment
#version 330 core
out vec4 FragColor;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

uniform vec3 u_viewPos;

struct Material {
	sampler2D diffuse1;
	sampler2D specular1;
	sampler2D emission1;
	float shininess;
};
uniform Material material;

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light;

uniform int u_b_ambient;
uniform int u_b_diffuse;
uniform int u_b_specular;

uniform bool u_b_blinn;
uniform bool u_b_specular_texture;

void main()
{
	// ambient
	vec3 ambient = light.ambient  * vec3(texture(material.diffuse1, fs_in.TexCoords)) * u_b_ambient;

	// diffuse
	vec3 norm = normalize(fs_in.Normal);
	vec3 lightDir = normalize(light.position - fs_in.FragPos);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = diff * vec3(texture(material.diffuse1, fs_in.TexCoords)) * u_b_diffuse;

	// specular
	vec3 viewDir = normalize(u_viewPos - fs_in.FragPos);
	float spec = 0.0;
	if (u_b_blinn)
	{
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(norm, halfwayDir), 0.0f), material.shininess); //32.0
	}
	else
	{
		vec3 reflectDir = reflect(-lightDir, norm);
		spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess); // 8.0
	}

	// Define if use a speculaMap or a vec3(1.0f) for specular-lighting
	vec3 specular;
	if (u_b_specular_texture)
		specular = light.specular * spec * vec3(texture(material.specular1, fs_in.TexCoords)) * u_b_specular;
	else
		specular = light.specular * spec  * u_b_specular;

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0f);
};

