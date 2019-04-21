#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;

out VS_OUT{
	vec3 WorldPos;
} vs_out;

uniform mat4 u_proj;
uniform mat4 u_view;

void main()
{
	vs_out.WorldPos = aPos;

	mat4 rotView = mat4(mat3(u_view));	// remove translation from the view matrix
	vec4 clipPos = u_proj * rotView * vec4(vs_out.WorldPos, 1.0f);

	gl_Position = clipPos.xyww;
};


#shader fragment
#version 330 core
out vec4 FragColor;

in VS_OUT{
	vec3 WorldPos;
} fs_in;

uniform samplerCube u_environmentMap;

void main()
{
	vec3 envColor = texture(u_environmentMap, fs_in.WorldPos).rgb;

	// HDR tonemap and gamma correct
	envColor = envColor / (envColor + vec3(1.0f));
	envColor = pow(envColor, vec3(1.0f / 2.2f));

	FragColor = vec4(envColor, 1.0f);
}
