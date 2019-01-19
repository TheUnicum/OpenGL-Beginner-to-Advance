#shader vertex
#version 330 core

layout(location = 0) in vec4 aPos;

out vec4 vertexColor; // specify a color output to the fragment shader

uniform vec4 ourColor;

void main()
{
	gl_Position = aPos;
	vertexColor = ourColor;// vec4(0.5f, 0.0f, 0.0f, 1.0f);
};

#shader fragment
#version 330 core

out vec4 FragColor;

in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)

void main()
{
	FragColor = vertexColor;
};

// VARIABLES Types
// int, float, double, uint, bool

// VECTORS TYPE
// vecn: the default vector of n floats.
// bvecn : a vector of n booleans.
// ivecn : a vector of n integers.
// uvecn : a vector of n unsigned integers.
// dvecn : a vector of n double components.