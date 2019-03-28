#shader vertex
#version 330 core

layout(location = 0) in vec4 aPos;
layout(location = 2) in vec2 aTexCoord;

out VS_to_FS{
	vec2 TexCoords;
} vs_out;


uniform mat4 u_mvp;

void main()
{
	gl_Position = u_mvp * aPos;
	gl_PointSize = gl_Position.z;
	vs_out.TexCoords = aTexCoord;
};

#shader fragment
#version 330 core

in VS_to_FS{
	vec2 TexCoords;
} fs_in;

out vec4 FragColor;

uniform int u_b_FrontFacingTest;
uniform int u_i_half_Width;

uniform sampler2D u_frontTexture;
uniform sampler2D u_backTexture;

void main()
{
	if (u_b_FrontFacingTest == 0)
	{
		if (gl_FragCoord.x < u_i_half_Width)
			FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		else
			FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	}
	else
	{
		if (gl_FrontFacing)
			FragColor = texture(u_frontTexture, fs_in.TexCoords);
		else
			FragColor = texture(u_backTexture, fs_in.TexCoords);
	}
};

