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

uniform int u_i_Tone_mapping_Type;
uniform float u_f_hdr_expose;

void main()
{
	const float gamma = 2.2;
	vec3 hdrColor = texture(screenTexture, TexCoords).rgb;
	
	if (u_i_Tone_mapping_Type == 0)
	{
		//FragColor = vec4(texture(screenTexture, TexCoords));
		FragColor = vec4(hdrColor, 1.0);
	}
	else if (u_i_Tone_mapping_Type == 1)
	{
		// reinhard tone mapping
		vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
		// gamma correction 
		mapped = pow(mapped, vec3(1.0 / gamma));

		FragColor = vec4(mapped, 1.0);
	}
	else if (u_i_Tone_mapping_Type == 2)
	{
		// reinhard tone mapping
		vec3 mapped = vec3(1.0) - exp(-hdrColor * u_f_hdr_expose);
		// gamma correction 
		mapped = pow(mapped, vec3(1.0 / gamma));

		FragColor = vec4(mapped, 1.0);
	}

};

