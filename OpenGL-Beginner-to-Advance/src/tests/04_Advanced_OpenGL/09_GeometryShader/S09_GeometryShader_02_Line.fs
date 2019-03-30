#version 330 core
out vec4 FragColor;

in GS_to_FS{
	vec3 Color;
} fs_in;

void main()
{
	FragColor = vec4(fs_in.Color, 1.0);
};
