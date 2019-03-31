#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;

out VS_OUT{
	vec2 texCoords;
} vs_out;

uniform mat4 u_mvp;

void main()
{
	vs_out.texCoords = aTexCoords;
	gl_Position = u_mvp * vec4(aPos, 1.0);
};


#shader geometry
#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT{
	vec2 texCoords;
} gs_in[];

out vec2 TexCoords;

uniform float u_time;

vec4 explode(vec4 position, vec3 normal)
{
	float magnitude = 2.0f;
	vec3 direction = normal * ((sin(u_time) + 1.0f) / 2.0f) * magnitude;
	return position + vec4(direction, 0.0f);
}

vec3 GetNormal()
{
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a, b));
}

void main()
{
	vec3 normal = GetNormal();

	gl_Position = explode(gl_in[0].gl_Position, normal);
	TexCoords = gs_in[0].texCoords;
	EmitVertex();
	gl_Position = explode(gl_in[1].gl_Position, normal);
	TexCoords = gs_in[1].texCoords;
	EmitVertex();
	gl_Position = explode(gl_in[2].gl_Position, normal);
	TexCoords = gs_in[2].texCoords;
	EmitVertex();
	EndPrimitive();
}


#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

struct Material {
	sampler2D diffuse1;
	sampler2D specular1;
	sampler2D emission1;
	float shininess;
};

uniform Material material;

void main()
{

	FragColor = vec4(vec3(texture(material.diffuse1, TexCoords)), 1.0f);
};
