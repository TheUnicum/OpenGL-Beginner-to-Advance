#shader vertex
#version 330 core

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec2 aTex;

out vec2 v_texCoord;	// specify a texture position output to the fragment shader

void main()
{
	gl_Position = aPos;
	v_texCoord = aTex;
};

#shader fragment
#version 330 core

in vec2 v_texCoord;
out vec4 FragColor;

uniform sampler2D u_Texture0;
uniform sampler2D u_Texture1;

uniform float u_mixFactor;

void main()
{
	FragColor = texture(u_Texture0, v_texCoord) * u_mixFactor + texture(u_Texture1, v_texCoord) * (1 - u_mixFactor);
};

