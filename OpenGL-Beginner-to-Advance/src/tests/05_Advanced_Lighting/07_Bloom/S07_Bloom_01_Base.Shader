#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} vs_out;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat3 u_transInvers_model;

void main()
{
	vs_out.FragPos = vec3(u_model * vec4(aPos, 1.0f)); // provare a mettere 0.0f
	vs_out.Normal = u_transInvers_model * aNormal;
	vs_out.TexCoords = aTexCoords;
	
	gl_Position = u_mvp * vec4(aPos, 1.0f);
};

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

struct Material {
	sampler2D diffuse1;
	sampler2D specular1;
	sampler2D emission1;
	float shininess;
};
uniform Material material;

struct Light {
	vec3 Position;
	vec3 Color;
};
uniform Light lights[16];

//uniform vec3 u_viewPos;

void main()
{
	//FragColor = vec4(texture(material.diffuse1, fs_in.TexCoords));

	vec3 color = texture(material.diffuse1, fs_in.TexCoords).rgb;
	vec3 normal = normalize(fs_in.Normal);
	// ambient
	vec3 ambient = 0.0 * color;
	// lighting
	vec3 lighting = vec3(0.0);
	for (int i = 0; i < 4; i++)
	{
		// diffuse
		vec3 lightDir = normalize(lights[i].Position - fs_in.FragPos);
		float diff = max(dot(lightDir, normal), 0.0);
		vec3 diffuse = lights[i].Color * diff * color;
		vec3 result = diffuse;
		// attenuation (use quadratic as we have gamma correction)
		float distance = length(fs_in.FragPos - lights[i].Position);
		result *= 1.0 / (distance * distance);
		lighting += result;

	}

	//FragColor = vec4(ambient + lighting, 1.0);
	vec3 result = ambient + lighting;
	// check whether result is higher than some threshold, if so, output as bloom threshold color
	float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0f)
		BrightColor = vec4(result, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	FragColor = vec4(result, 1.0f);

};

