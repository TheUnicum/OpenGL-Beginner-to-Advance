#shader vertex
#version 330 core

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec2 aTex;

out vec2 v_texCoord;	// specify a texture position output to the fragment shader

uniform float u_scale;
float invers_scale = 1 / (u_scale);

void main()
{
	gl_Position = aPos;
	//v_texCoord = aTex;
	float f_ratio = (2 * invers_scale + 2 / (invers_scale + 1));
	v_texCoord = vec2(aTex.x * f_ratio - (invers_scale), aTex.y * f_ratio - (invers_scale));
};

#shader fragment
#version 330 core

in vec2 v_texCoord;
out vec4 FragColor;

uniform sampler2D u_Texture;

void main()
{
	FragColor = texture(u_Texture, v_texCoord);
};

