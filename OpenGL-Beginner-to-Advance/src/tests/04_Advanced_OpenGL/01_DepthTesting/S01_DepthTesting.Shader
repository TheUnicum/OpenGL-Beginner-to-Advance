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
uniform int u_b_emission;

uniform int u_i_DP_Visualizing_opt;
float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // back to NDC 
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
	// ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse1, TexCoords)) * u_b_ambient;

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse1, TexCoords)) * u_b_diffuse;

	// specular
	vec3 viewDir = normalize(u_viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords)) * u_b_specular;

	// emission
	vec3 emission = texture(material.emission1, TexCoords).rgb * u_b_emission;

	vec3 result = ambient + diffuse + specular + emission;

	if (u_i_DP_Visualizing_opt == 0)
		FragColor = vec4(result, 1.0f);
	else if(u_i_DP_Visualizing_opt == 1)
		FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
	else
	{
		float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
		FragColor = vec4(vec3(depth), 1.0);
	}
};

