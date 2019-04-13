#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} vs_out;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat3 u_transInvers_model;

uniform bool u_b_inverse_normals;

void main()
{
	vs_out.FragPos = vec3(u_view * u_model * vec4(aPos, 1.0f)); // provare a mettere 0.0f
	vs_out.Normal = u_transInvers_model * (u_b_inverse_normals ? -aNormal : aNormal); // aNormal;
	vs_out.TexCoords = aTexCoords;

	gl_Position = u_mvp * vec4(aPos, 1.0f);
};


#shader fragment
#version 330 core
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec3 gAlbedo;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

void main()
{
	// store the fragment position vector in the first gbuffer texture
	gPosition = fs_in.FragPos;
	// also store the per-fragment normals into the gbuffer
	gNormal = normalize(fs_in.Normal);
	// and the diffuse per-fragment color
	gAlbedo.rgb = vec3(0.95f);

};

