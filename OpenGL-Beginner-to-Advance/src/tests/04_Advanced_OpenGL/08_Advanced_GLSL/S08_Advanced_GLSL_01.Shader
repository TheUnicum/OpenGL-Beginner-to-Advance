#shader vertex
#version 330 core

layout(location = 0) in vec4 aPos;

uniform mat4 u_mvp;

void main()
{
	gl_Position = u_mvp * aPos;
	gl_PointSize = gl_Position.z;
};

#shader fragment
#version 330 core
out vec4 FragColor;

void main()
{
	FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
};

