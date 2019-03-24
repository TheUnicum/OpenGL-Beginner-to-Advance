#shader vertex
#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);
};

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int u_b_pproc_Inversion;

void main()
{
	if (u_b_pproc_Inversion == 1)
		FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
	else
		FragColor = vec4(texture(screenTexture, TexCoords));
};

