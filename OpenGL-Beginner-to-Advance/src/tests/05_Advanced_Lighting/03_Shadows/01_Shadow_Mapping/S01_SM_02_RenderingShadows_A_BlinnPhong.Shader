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
uniform mat4 u_model;
uniform mat3 u_transInvers_model;

void main()
{
	vs_out.FragPos = vec3(u_model * vec4(aPos, 1.0f)); // should be multiply by mat4:model
	vs_out.Normal = u_transInvers_model * aNormal;
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
	vec3 direction;

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
	vec3 ambient = light.ambient * vec3(texture(material.diffuse1, fs_in.TexCoords)) * u_b_ambient;

	// diffuse
	vec3 norm = normalize(fs_in.Normal);
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse1, fs_in.TexCoords)) * u_b_diffuse;

	// specular
	vec3 viewDir = normalize(u_viewPos - fs_in.FragPos);	
	float spec;
	if (u_b_blinn)
	{	//Blinn-Phong
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(norm, halfwayDir), 0.0f), material.shininess*4); //32.0
	}
	else
	{	//Phong
		vec3 reflectDir = reflect(-lightDir, norm);
		spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	}

	// Define if use a speculaMap or a vec3(1.0f) for specular-lighting
	vec3 specular;
	if (u_b_specular_texture)
		specular = light.specular * spec * vec3(texture(material.specular1, fs_in.TexCoords)) * u_b_specular;
	else
		specular = light.specular * spec  * u_b_specular;

	// emission
	//vec3 emission = texture(material.emission1, fs_in.TexCoords).rgb * u_b_emission;

	vec3 result = ambient + diffuse + specular;// +emission;
	FragColor = vec4(result, 1.0f);

};

