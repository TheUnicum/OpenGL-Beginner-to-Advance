#shader vertex
#version 330 core

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec4 aColor;

out vec4 vertexColor; // specify a color output to the fragment shader

void main()
{
	gl_Position = aPos;
	vertexColor = aColor;
};

#shader fragment
#version 330 core

out vec4 FragColor;

in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)

void main()
{
	FragColor = vertexColor;
};