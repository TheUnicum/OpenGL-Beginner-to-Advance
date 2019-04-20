#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} vs_out;

uniform mat4 u_mvp;
uniform mat4 u_lightSpaceMatrix; // projLight * viewLight
uniform mat4 u_model;
uniform mat3 u_transInvers_model;

void main()
{
	vs_out.FragPos = vec3(u_model * vec4(aPos, 1.0f));
	vs_out.Normal = u_transInvers_model * aNormal;
	vs_out.TexCoords = aTexCoords;
	//vs_out.FragPosLightSpace = u_mvp_LightSpace * vec4(aPos, 1.0f);
	vs_out.FragPosLightSpace = u_lightSpaceMatrix * vec4(vs_out.FragPos, 1.f);
	gl_Position = u_mvp * vec4(aPos, 1.0f);
};


#shader fragment
#version 330 core
out vec4 FragColor;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
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

uniform sampler2D u_shadowMap;

uniform bool u_b_gamma;
uniform float u_f_gamma;
uniform bool u_b_quadratic;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	// preform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// trasform to [0, 1] range
	projCoords = projCoords * 0.5f + 0.5f;
	// get closest depth value from light's prospective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(u_shadowMap, projCoords.xy).r;
	// get depth of current fragment from light' perspective
	float currentDepth = projCoords.z;
	// check whether current frag pos is in shadow

	// 1- No Bias
	//float shadow = currentDepth > closestDepth ? 1.0f : 0.0f;
	
	// 2- Static Bias
	//float bias = .05f;
	//float shadow = currentDepth - bias > closestDepth ? 1.f : 0.f;

	// 3- Calculate Bias (based on depth map resolution and slope)
	vec3 normal = normalize(fs_in.Normal);
	float bias = max(0.05 * (1.0 - dot(normal, -light.direction)), 0.005);
	//float shadow = currentDepth - bias > closestDepth ? 1.f : 0.f;

	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0);
	//for (int x = -1; x <= 1; ++x)
	for (int x = -2; x <= 2; ++x)
	{
		for (int y = -2; y <= 2; ++y)
		{
			float pcfDepth = texture(u_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 25.0;// 9.0;

	// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
	if (projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}

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
		spec = pow(max(dot(norm, halfwayDir), 0.0f), material.shininess * 4); //32.0
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

	// Calculate shadow
	float shadow = ShadowCalculation(fs_in.FragPosLightSpace);


	vec3 result = ambient + (1.0f - shadow) * (diffuse + specular);// +emission;
	//vec3 result = (ambient + (1.f - shadow) * (diffuse + specular));


	if (u_b_gamma)
	{
		float gamma = u_f_gamma;// 2.2;
		result = pow(result, vec3(1.0 / gamma));
	}

	FragColor = vec4(result, 1.0f);

};

