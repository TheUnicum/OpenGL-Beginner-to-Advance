#shader vertex
#version 330 core

layout(location = 0) in vec4 aPos;

uniform mat4 u_mvp;

void main()
{
	gl_Position = u_mvp * aPos;
};

#shader fragment
#version 330 core

out vec4 FragColor;

uniform vec3 u_lightColor;

void main()
{
	FragColor = vec4(u_lightColor, 1.0f);
};

