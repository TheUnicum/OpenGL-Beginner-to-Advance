#shader vertex
#version 330 core

layout(location = 0) in vec4 aPos;

out vec3 TexCoords;

uniform mat4 u_mvp;
uniform int u_b_optimization;

void main()
{
	TexCoords = vec3(aPos);
	vec4 pos = u_mvp * aPos;
	if (u_b_optimization == 0)
		gl_Position = pos.xyzw;
	else
		gl_Position = pos.xyww;
};

#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
	FragColor = texture(skybox, TexCoords);
};

