#shader vertex
#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);
};

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int u_i_postproc_effect;


void main()
{
	// kernel
	const float offset = 1.0 / 300.0;
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset), // top-left
		vec2(0.0f, offset), // top-center
		vec2(offset, offset), // top-right
		vec2(-offset, 0.0f),   // center-left
		vec2(0.0f, 0.0f),   // center-center
		vec2(offset, 0.0f),   // center-right
		vec2(-offset, -offset), // bottom-left
		vec2(0.0f, -offset), // bottom-center
		vec2(offset, -offset)  // bottom-right    
		);

	float kernel[9];
	// Kernel A
	if (u_i_postproc_effect == 4)
		kernel = float[](
			-1, -1, -1,
			-1, 9, -1,
			-1, -1, -1
			);
	// Kernel B
	if (u_i_postproc_effect == 5)
		kernel = float[](
			+2, +2, +2,
			+2, -15, +2,
			+2, +2, +2
			);
	// Blur
	if (u_i_postproc_effect == 6)
		kernel = float[](
			+1.0 / 16, +2.0 / 16, +1.0 / 16,
			+2.0 / 16, -5.0 / 16, +2.0 / 16,
			+1.0 / 16, +2.0 / 16, +1.0 / 16
			);
	// Edge detection
	if (u_i_postproc_effect == 7)
		kernel = float[](
			+1, +1, +1,
			+1, -8, +1,
			+1, +1, +1
			);


	if (u_i_postproc_effect == 1)
		FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
	else if (u_i_postproc_effect == 2)
	{
		FragColor = vec4(texture(screenTexture, TexCoords));
		float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0f;
		FragColor = vec4(average, average, average, 1.0f);
	}
	else if (u_i_postproc_effect == 3)
	{
		FragColor = vec4(texture(screenTexture, TexCoords));
		float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
		FragColor = vec4(average, average, average, 1.0f);
	}
	else if (u_i_postproc_effect > 3 && u_i_postproc_effect < 10)
	{
		vec3 sampleTex[9];
		for (int i = 0; i < 9; i++)
			sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
		vec3 col = vec3(0.0f);
		for (int i = 0; i < 9; i++)
			col += sampleTex[i] * kernel[i];
		FragColor = vec4(col, 1.0f);
	}
	else
		FragColor = vec4(texture(screenTexture, TexCoords));
};

