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
