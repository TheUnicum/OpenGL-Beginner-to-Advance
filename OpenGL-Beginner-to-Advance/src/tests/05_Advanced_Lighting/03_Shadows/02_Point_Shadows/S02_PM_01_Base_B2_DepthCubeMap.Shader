#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 u_model;

void main()
{
	gl_Position = u_model * vec4(aPos, 1.0f);
};


#shader geometry
#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;	//could be also just 3.... try to change

uniform mat4 u_shadowMatrices[6];

out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main()
{
	for (int face = 0; face < 6; ++face)
	{
		gl_Layer = face; // built-in variable that specifies to which face we render.
		for (int i = 0; i < 3; ++i) // for each triangle's vertex
		{
			FragPos = gl_in[i].gl_Position;
			gl_Position = u_shadowMatrices[face] * FragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}


#shader fragment
#version 330 core
out vec4 FragColor;

in vec4 FragPos;

uniform vec3 u_lightPos;
uniform float u_far_plane;

void main()
{
	// get distance between fragment and light source
	float lightDistance = length(FragPos.xyz - u_lightPos);

	// map to [0;1] range by division by far_plane
	lightDistance = lightDistance / u_far_plane;

	// write this as modified depth
	gl_FragDepth = lightDistance;	// gl_FragCoord.z;

	// set color for debugging purpoise
	FragColor = vec4(vec3(gl_FragDepth), 1.0);
};

