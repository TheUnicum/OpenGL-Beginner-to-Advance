#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} vs_out;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat3 u_transInvers_model;

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Light light;
uniform vec3 u_viewPos;

void main()
{
	vs_out.FragPos = vec3(u_model * vec4(aPos, 1.0f)); // provare a mettere 0.0f
	vs_out.Normal = u_transInvers_model * aNormal;
	vs_out.TexCoords = aTexCoords;

	vec3 T = normalize(u_transInvers_model * aTangent);
	vec3 N = normalize(u_transInvers_model * aNormal);
	// re-orthogonalize T with respect to N
	// Gram-Schmidt process to re-orthogonaliz
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	mat3 TBN = transpose(mat3(T, B, N));
	vs_out.TangentLightPos = TBN * light.position;
	vs_out.TangentViewPos = TBN * u_viewPos;
	vs_out.TangentFragPos = TBN * vs_out.FragPos;

	gl_Position = u_mvp * vec4(aPos, 1.0f);// aPos;
};

#shader fragment
#version 330 core
out vec4 FragColor;


in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} fs_in;


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

uniform float u_f_heightScale;

uniform bool u_b_discardFragment_outside;
uniform bool u_b_OffsetLimit;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
	float height = texture(material.height1, texCoords).r;

	//Parallax Mapping with Offset Limiting
	if (!u_b_OffsetLimit)
		return texCoords - viewDir.xy * (height * u_f_heightScale);
	else
		return texCoords - viewDir.xy / viewDir.z * (height * u_f_heightScale);
}

void main()
{
	// Make all the lights calculation in Tangent-Space

	// offset texture coordinates with Parallax Mapping
	vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
	vec2 act_texCoords = fs_in.TexCoords;

	act_texCoords = ParallaxMapping(act_texCoords, viewDir);

	// discard the fragment whenever it samples outside the default texture coordinate range
	if ((act_texCoords.x > 1.0f || act_texCoords.y > 1.0f || act_texCoords.x < 0.0f || act_texCoords.y < 0.0f)
		&& (u_b_discardFragment_outside))
		discard;

	// obtain normal map in range [0, 1]
	vec3 normal = texture(material.normal1, act_texCoords).rgb;
	// trasnsform normal vector to range [-1, +1]
	normal = normalize(normal * 2 - 1.0f);

	// Option Active Normal & NormalMap
	vec3 norm;
	if (u_i_NormalMap_opt == 0)
		norm = normalize(fs_in.Normal);
	else if (u_i_NormalMap_opt == 1)
		norm = normalize(normal);
	else if (u_i_NormalMap_opt == 2)
	{
		if (gl_FragCoord.x < u_i_half_Width)
			norm = normalize(normal);
		else
			norm = normalize(fs_in.Normal);
	}

	//----------------------------

	// ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse1, act_texCoords)) * u_b_ambient;

	// diffuse
	vec3 NormalActive;
	if (u_b_NorMap_diffuse)
		NormalActive = norm;
	else
		NormalActive = normalize(fs_in.Normal);

	//vec3 lightDir = normalize(light.position - FragPos);
	vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
	float diff = max(dot(NormalActive, lightDir), 0.0f);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse1, act_texCoords)) * u_b_diffuse;

	// specular
	if (u_b_NorMap_specular)
		NormalActive = norm;
	else
		NormalActive = normalize(fs_in.Normal);

	//vec3 viewDir = normalize(u_viewPos - FragPos);
	//----------> vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
	vec3 reflectDir = reflect(-lightDir, NormalActive);  // Normal
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(NormalActive, halfwayDir), 0.0), material.shininess); // 32.0 //Normal
	vec3 specular = light.specular * spec * u_b_specular;

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0f);
};
