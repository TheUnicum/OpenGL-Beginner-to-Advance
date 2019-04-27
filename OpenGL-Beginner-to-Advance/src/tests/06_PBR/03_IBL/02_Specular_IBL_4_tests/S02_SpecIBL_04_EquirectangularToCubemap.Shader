#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;

out VS_OUT{
	vec3 WorldPos;
} vs_out;

uniform mat4 u_mvp;		//projection * view

void main()
{
	vs_out.WorldPos = aPos;
	gl_Position = u_mvp * vec4(aPos, 1.0f);
};


#shader fragment
#version 330 core
out vec4 FragColor;

in VS_OUT{
	vec3 WorldPos;
} fs_in;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += 0.5; 
	return uv;
}

void main()
{
	vec2 uv = SampleSphericalMap(normalize(fs_in.WorldPos)); // Make sure to normalize WorldPos
	vec3 color = texture(equirectangularMap, uv).rgb;

	FragColor = vec4(color, 1.0f);
}
