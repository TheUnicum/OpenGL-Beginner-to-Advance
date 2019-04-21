#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out VS_OUT{
	vec3 WorldPos;
	vec3 Normals;
	vec2 TexCoords;
} vs_out;

uniform mat4 u_model;
uniform mat4 u_mvp;

void main()
{
	vs_out.WorldPos = vec3(u_model * vec4(aPos, 1.0f));
	vs_out.Normals = mat3(u_model) * aNormal;
	vs_out.TexCoords = aTexCoords;

	gl_Position = u_mvp * vec4(aPos, 1.0f);
};




// -30 for debuf errors...
#shader fragment
#version 330 core

out vec4 FragColor;

in VS_OUT{
	vec3 WorldPos;
	vec3 Normals;
	vec2 TexCoords;
} fs_in;

// material parameters
uniform vec3 u_albedo;
uniform float u_metallic;
uniform float u_roughtness;
uniform float u_ao;

// lights
uniform vec3 u_lightPositions[4];
uniform vec3 u_lightColors[4];

uniform vec3 u_camPos;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0f);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
	denom = PI * denom * denom;

	return nom / denom; // max(denom, 0.001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0f);
	float k = (r * r) / 8.0f;

	float nom = NdotV;
	float denom = NdotV * (1.0f - k) + k;

	return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0f);
	float NdotL = max(dot(N, L), 0.0f);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1 - F0) * pow(1.0f - cosTheta, 5.0f);
}
// ----------------------------------------------------------------------------
void main()
{
	vec3 N = normalize(fs_in.Normals);
	vec3 V = normalize(u_camPos - fs_in.WorldPos);

	// calcutate reflectance at normal incidence; if dia-electric (like plastic) use F0
	// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
	vec3 F0 = vec3(0.04f);
	F0 = mix(F0, u_albedo, u_metallic);

	// reflectance equation
	vec3 Lo = vec3(0.0f);
	for (int i = 0; i < 4; i++)
	{
		// calculate per-light radiance
		vec3 L = normalize(u_lightPositions[i] - fs_in.WorldPos);
		vec3 H = normalize(V + L);
		float distance = length(u_lightPositions[i] - fs_in.WorldPos);
		float attenuation = 1.0f / (distance * distance);
		vec3 radiance = u_lightColors[i] * attenuation;

		// Cook-Torrance BRDF
		float NDF = DistributionGGX(N, H, u_roughtness);
		float G = GeometrySmith(N, V, L, u_roughtness);
		vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0f, 1.0f), F0);

		vec3 nominator = NDF * G * F;
		float denominator = 4 * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f);
		vec3 specular = nominator / max(denominator, 0.001); // prevent divide by zero fro NdotV=0.0 or NdotL=0.0

		// kS is equal to Fresnel
		vec3 kS = F;
		// for energy conservation, the diffuse and specular light can't
		// be above 1.0 (unless the surface emits light); to preserve this
		// relationship the diffuse component (kD) should equal 1.0 - kS.
		vec3 kD = vec3(1.0f) - kS;
		// multiply kD by the inverse metalness such that only non-metals
		// have diffuse lighting, or a linear blend if partly metal (pure metals
		// have no diffuse light).
		kD *= 1.0f - u_metallic;

		// scale light by NdotL
		float NdotL = max(dot(N, L), 0.0f);

		// add to outgoing radiance Lo
		Lo += (kD * u_albedo / PI + specular) * radiance * NdotL;	// note that we already multiply the BRDF
		//by the Fresnel (kS) so we won't multiply by kS again
	}

	// ambient lighting (note that the nect IBL tutorial will replace
	// this ambient lighting with environment lighting).
	vec3 ambient = vec3(0.03f) * u_albedo * u_ao;

	vec3 color = ambient + Lo;

	// HDR tonemapping
	color = color / (color + vec3(1.0f));
	// gamma correct
	color = pow(color, vec3(1.0f / 2.2f));

	FragColor = vec4(color, 1.0f);
};

