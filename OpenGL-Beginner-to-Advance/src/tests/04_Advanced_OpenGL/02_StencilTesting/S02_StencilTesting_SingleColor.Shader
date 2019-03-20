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

void main()
{
	FragColor = vec4(0.04f, 0.28f, 0.26f, 1.0f);
};

