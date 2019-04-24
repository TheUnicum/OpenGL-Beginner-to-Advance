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

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main()
{
	// The world vector acts as the normal of a tangent surface
	// from the origin, aligned to WorldPos. Given this normal, calculate all
	// incoming radiance of the environment. The result of this radiance
	// is the radiance of light coming from -Normal direction, which is what
	// we use in the PBR shader to sample irradiance.
	vec3 N = normalize(fs_in.WorldPos);

	vec3 irradiance = vec3(0.0f);

	// tangent space calculation from origin point
	vec3 up = vec3(0.0f, 1.0f, 0.0f);
	vec3 right = cross(up, N);
	up = cross(N, right);

	float sampleDelta = 0.025;
	float nrSamples = 0.0f;
	for (float phi = 0.0f; phi < 2.0f * PI; phi += sampleDelta)
	{
		for (float theta = 0.0f; theta < 0.5f * PI; theta += sampleDelta)
		{
			// spherical to cartesian (in tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			// tangent space to world
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

			irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	irradiance = PI * irradiance * (1.0f / float(nrSamples));

	FragColor = vec4(irradiance, 1.0f);
}
