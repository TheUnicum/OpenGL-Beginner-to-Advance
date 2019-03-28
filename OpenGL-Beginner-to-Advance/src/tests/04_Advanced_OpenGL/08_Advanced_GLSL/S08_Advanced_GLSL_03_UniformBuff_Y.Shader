#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;

layout(std140) uniform Matrices
{
	mat4 u_projection;
	mat4 u_view;
};
uniform mat4 u_model;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(aPos, 1.0f);
};


#shader fragment
#version 330 core

out vec4 FragColor;

void main()
{
	FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
};

