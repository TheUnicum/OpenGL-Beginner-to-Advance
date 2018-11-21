#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

out vec4 v_color;

void main()
{
	gl_Position = position;
	v_color = color;
}
;

#shader fragment
#version 330 core

in vec4 v_color;
out vec4 color;

uniform vec4 u_Color;

void main()
{
	color = v_color + u_Color;
};