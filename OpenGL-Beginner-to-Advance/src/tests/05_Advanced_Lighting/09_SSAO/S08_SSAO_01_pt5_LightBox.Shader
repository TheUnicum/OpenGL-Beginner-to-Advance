#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 u_mvp;

void main()
{
	gl_Position = u_mvp * vec4(aPos, 1.0f);
};

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;

uniform vec3 u_lightColor;

void main()
{
	FragColor = vec4(u_lightColor, 1.0f);
};
