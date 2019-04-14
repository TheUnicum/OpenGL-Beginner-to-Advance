#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = vec4(aPos, 1.0f);
};


#shader fragment
#version 330 core
//out float FragColor;
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[256]; // 64

// parameters (you'd probably want to use them as uniforms to more easily tweak effect)
uniform int u_i_kernelSize;	//int kernelSize = 64;
uniform float u_f_radius;	//float radius = 0.5;
uniform float u_f_bias;		//float bias = 0.025;

uniform int u_i_power;

// tile noise texture over sceen based on screen dimension divided by noise size
uniform vec2 u_noiseScale;	//const vec2 noiseScale = vec2(960.0f / 4.0f, 540.0f / 4.0f);

uniform mat4 u_proj;

void main()
{
	// get input for SSAO algorithm
	vec3 fragPos = texture(gPosition, TexCoords).xyz;
	vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
	vec3 randomVec = normalize(texture(texNoise, TexCoords * u_noiseScale).xyz);

	// create TBN change-of-basis matrix: from tangent-space to view-space
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	// iterate over the sample kernel and calculate occlusion factor
	float occlusion = 0.0f;
	for (int i = 0; i < u_i_kernelSize; i++)
	{
		// get sample position
		vec3 sample = TBN * samples[i]; // from tangent to view-space
		sample = fragPos + sample * u_f_radius;

		// project sample position (to sample texture) (to get position on screen/texture)
		vec4 offset = vec4(sample, 1.0f);
		offset = u_proj * offset; // from view to clip-space
		offset.xyz /= offset.w; // perspective divide
		offset.xyz = offset.xyz * 0.5f + 0.5f; // transform to range 0.0 - 1.0

		// get sample depth
		float sampleDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample

		// range check & accumulate
		float rangeCheck = smoothstep(0.0, 1.0, u_f_radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= sample.z + u_f_bias ? 1.0f : 0.0f) * rangeCheck;
	}
	occlusion = 1.0f - (occlusion / u_i_kernelSize);
	occlusion = pow(occlusion, u_i_power);
	FragColor = vec4(occlusion);
};

