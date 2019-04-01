#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;

out VS_OUT{
	vec2 texCoords;
} vs_out;

uniform mat4 u_view;
uniform mat4 u_proj;

void main()
{
	vs_out.texCoords = aTexCoords;
	gl_Position = u_proj * u_view * instanceMatrix * vec4(aPos, 1.0);
};


#shader fragment
#version 330 core
out vec4 FragColor;

in VS_OUT{
	vec2 texCoords;
} fs_in;

struct Material {
	sampler2D diffuse1;
	sampler2D specular1;
	sampler2D emission1;
	float shininess;
};

uniform Material material;

void main()
{
	FragColor = vec4(vec3(texture(material.diffuse1, fs_in.texCoords)), 1.0f);
};
