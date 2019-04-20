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
uniform mat4 u_lightSpaceMatrix; // projLight * viewLight
uniform mat4 u_model;
uniform mat3 u_transInvers_model;

uniform bool u_b_inverse_normals;

void main()
{
	vs_out.FragPos = vec3(u_model * vec4(aPos, 1.0f));
	vs_out.Normal = u_transInvers_model * (u_b_inverse_normals ? -aNormal : aNormal);// aNormal;
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
	//vec4 FragPosLightSpace;
} fs_in;

uniform vec3 u_viewPos;

struct Material {
	sampler2D diffuse1;
	sampler2D specular1;
	sampler2D emission1;
	float shininess;
};
uniform Material material;
uniform samplerCube u_depthMap;

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

uniform float u_far_plane;
uniform bool u_shadows;
uniform int u_i_PCF_type;


// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
	vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
	);

float ShadowCalculation(vec3 fragPos)
{
	// get vector between fragment position and light position
	vec3 fragToLight = fragPos - light.position;
	// use the fragment to light vector to sample from the depth map    
	float closestDepth = texture(u_depthMap, fragToLight).r;
	// it is currently in linear range between [0,1], let's re-transform it back to original depth value
	closestDepth *= u_far_plane;
	// now get current linear depth as the length between the fragment and light position
	float currentDepth = length(fragToLight);
	// test for shadows
	float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	// display closestDepth as debug (to visualize depth cubemap)
	// FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    

	// PCF
	if (u_i_PCF_type == 1)
	{
		shadow = 0.0f;
		float bias = 0.05;
		float samples = 4.0f;
		float offset = 0.1f;
		for (float x = -offset; x < offset; x += offset / (samples * 0.5))
		{
			for (float y = -offset; y < offset; y += offset / (samples * 0.5))
			{
				for (float z = -offset; z < offset; z += offset / (samples * 0.5))
				{
					float closestDepth = texture(u_depthMap, fragToLight + vec3(x, y, z)).r;
					closestDepth *= u_far_plane;   // Undo mapping [0;1]
					if (currentDepth - bias > closestDepth)
						shadow += 1.0;
				}
			}
		}
		shadow /= (samples * samples * samples);
	}


	// PCF - Optimized
	if (u_i_PCF_type == 2)
	{
		shadow = 0.0f;
		float bias = 0.05;
		int i_samples = 20;
		float viewDistance = length(u_viewPos - fragPos);
		// making the shadows softer when far away and sharper when close by.
		float diskRadius = (1.0 + (viewDistance / u_far_plane)) / 25.0;
		for (int i = 0; i < i_samples; ++i)
		{
			float closestDepth = texture(u_depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
			closestDepth *= u_far_plane;   // undo mapping [0;1]
			if (currentDepth - bias > closestDepth)
				shadow += 1.0;
		}
		shadow /= float(i_samples);

	}

	return shadow;

}

void main()
{
	// ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse1, fs_in.TexCoords)) * u_b_ambient;
	
	// diffuse
	vec3 norm = normalize(fs_in.Normal);
	vec3 lightDir = normalize(light.position - fs_in.FragPos);
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
	float shadow = u_shadows ? ShadowCalculation(fs_in.FragPos) : 0.0;
	
	
	vec3 result = ambient + (1.0f - shadow) * (diffuse + specular);// +emission;
	//vec3 result = (ambient + (1.f - shadow) * (diffuse + specular));
	
	
	if (u_b_gamma)
	{
		float gamma = u_f_gamma;// 2.2;
		result = pow(result, vec3(1.0 / gamma));
	}
	
	FragColor = vec4(result, 1.0f);

};

