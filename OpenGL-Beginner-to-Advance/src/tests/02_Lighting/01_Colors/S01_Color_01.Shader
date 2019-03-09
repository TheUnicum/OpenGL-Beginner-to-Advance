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

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
	FragColor = vec4(lightColor * objectColor, 1.0f);
};

