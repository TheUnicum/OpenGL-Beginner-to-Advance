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

out vec4 FragColor;

void main()
{
	FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
};

