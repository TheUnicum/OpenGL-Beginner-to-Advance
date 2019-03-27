#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;


out vec3 Normal;
out vec3 Position;
out vec2 TexCoords;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat3 u_transInvers_model;

void main()
{
	Normal = u_transInvers_model * aNormal;
	Position = vec3(u_model * vec4(aPos, 1.0f));
	TexCoords = aTexCoords;

	gl_Position = u_mvp * vec4(aPos, 1.0f);
};

#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;

struct Material {
	sampler2D diffuse1;
	sampler2D specular1;
	sampler2D emission1;
	float shininess;
};

uniform Material material;

uniform vec3 u_viewPos;
uniform samplerCube skybox;
uniform int u_b_cube_boxtexture;
uniform int u_b_refraction;
uniform float u_f_transparency;

void main()
{
	if (u_b_cube_boxtexture == 1)
	{
		FragColor = vec4(texture(material.diffuse1, TexCoords));
		//vec3 I = normalize(Position - u_viewPos);
		//vec3 R = reflect(I, normalize(Normal));
		//FragColor = vec4(texture(skybox, R).rgb, 1.0f);
		//FragColor = vec4(1.0f);
	}
	else
	{
		if (u_b_refraction == 0)
		{
			vec3 I = normalize(Position - u_viewPos);
			vec3 R = reflect(I, normalize(Normal));
			FragColor = vec4(texture(skybox, R).rgb, 1.0f);
		}
		else
		{
			float ratio = 1.00 / u_f_transparency;
			vec3 I = normalize(Position - u_viewPos);
			vec3 R = refract(I, normalize(Normal), ratio);
			FragColor = vec4(texture(skybox, R).rgb, 1.0f);
		}
	}
};

