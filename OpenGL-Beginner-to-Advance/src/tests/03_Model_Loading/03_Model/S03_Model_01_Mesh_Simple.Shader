#shader vertex
#version 440 core

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
#version 440

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
	FragColor = texture(texture_diffuse1, TexCoords);
}


