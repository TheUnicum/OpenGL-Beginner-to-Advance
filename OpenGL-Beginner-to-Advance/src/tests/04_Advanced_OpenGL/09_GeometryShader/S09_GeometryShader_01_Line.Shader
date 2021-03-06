#shader vertex
#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;

out VS_to_GS{
	vec3 Color;
} vs_out;

void main()
{
	gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);
	vs_out.Color = aColor;
};

#shader geometry
#version 330 core
layout(points) in;
layout(line_strip, max_vertices = 2) out;

in VS_to_GS{
	vec3 Color;
} gs_in[];

out GS_to_FS{
	vec3 Color;
} gs_out;

void main() 
{
	gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0f, 0.0f, 0.0f);
	gs_out.Color = gs_in[0].Color;
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(+0.1, 0.0f, 0.0f, 0.0f);
	gs_out.Color = gs_in[0].Color;
	EmitVertex();

	EndPrimitive();
}

#shader fragment
#version 330 core
out vec4 FragColor;

in GS_to_FS{
	vec3 Color;
} fs_in;

void main()
{
	FragColor = vec4(fs_in.Color, 1.0);
};
