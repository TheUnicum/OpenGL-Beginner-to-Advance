#shader vertex
#version 330 core

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 u_mvp;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = u_mvp * aPos;
};

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

struct Material {
	sampler2D diffuse1;
	sampler2D specular1;
	sampler2D emission1;
	float shininess;
};

uniform Material material;

void main()
{
	FragColor = vec4(texture(material.diffuse1, TexCoords));
};
