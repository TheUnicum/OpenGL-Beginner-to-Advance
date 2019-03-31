#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out VS_OUT{
	vec3 normal;
	vec2 texCoords;
} vs_out;

uniform mat4 u_view;
uniform mat4 u_proj;
uniform mat4 u_mvp;
uniform mat3 u_transInvers_model;

void main()
{
	vs_out.normal = normalize(vec3(u_proj * u_view *  vec4(u_transInvers_model * aNormal, 0.0)));
	vs_out.texCoords = aTexCoords;
	gl_Position = u_mvp * vec4(aPos, 1.0);
};


#shader geometry
#version 330 core
layout(triangles) in;
layout(line_strip, max_vertices = 2) out;

in VS_OUT{
	vec3 normal;
	vec3 texCoords;
} gs_in[];

const float MAGNITUDE = 0.4;

void GenerateLine(int index)
{
	gl_Position = gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE;
	EmitVertex();
	EndPrimitive();
}

void main()
{
	GenerateLine(0); // first vertex normal
	GenerateLine(1); // second vertex normal
	GenerateLine(2); // third vertex normal
}


#shader fragment
#version 330 core

out vec4 FragColor;

void main()
{
	FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}
