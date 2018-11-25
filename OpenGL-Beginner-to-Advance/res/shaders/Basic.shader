#shader vertex
#version 330 core

layout(location = 0) in vec4 vs_position;
layout(location = 1) in vec4 vs_color;
layout(location = 2) in vec2 vs_texCoord;

out vec4 v_color;
out vec2 v_texCoord;

void main()
{
	gl_Position = vs_position;
	v_color = vs_color;
	v_texCoord = vs_texCoord;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 fs_color;

in vec4 v_color;
in vec2 v_texCoord;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main()
{
	vec4 texColor = texture(u_Texture, v_texCoord);
	fs_color = texColor + vec4(u_Color.xyz, 0.0f);
};