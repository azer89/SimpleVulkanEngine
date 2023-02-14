#version 450

layout (constant_id = 0) const int NUM_LIGHT = 1;

layout (binding = 1) uniform sampler2D texSampler;

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;
layout (location = 3) in vec2 inTexCoord;

layout (location = 0) out vec4 outColor;

struct PointLight 
{
	vec4 position; // ignore w
	vec4 color; // w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo 
{
	mat4 projection;
	mat4 view;
	mat4 inverseView;
	vec4 ambientLightColor; // w is intensity
	PointLight pointLights[NUM_LIGHT];
} ubo;

// TODO change into a single descriptor set
layout(push_constant) uniform Push 
{
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main() 
{
	vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	vec3 specularLight = vec3(0.0);
	vec3 surfaceNormal = normalize(fragNormalWorld);
	vec3 cameraPosWorld = ubo.inverseView[3].xyz;
	vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

	for (int i = 0; i < NUM_LIGHT; i++) 
	{
		PointLight light = ubo.pointLights[i];
		vec3 directionToLight = light.position.xyz - fragPosWorld;
		float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared
		directionToLight = normalize(directionToLight);

		// diffuse
		float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
		vec3 intensity = light.color.xyz * light.color.w * attenuation;
		diffuseLight += intensity * cosAngIncidence;

		// specular
		vec3 halfAngle = normalize(directionToLight + viewDirection);
		float blinnTerm = dot(surfaceNormal, halfAngle);
		blinnTerm = clamp(blinnTerm, 0, 1);
		blinnTerm = pow(blinnTerm, 512.0); // higher values -> sharper highlight
		specularLight += intensity * blinnTerm;
	}

	vec4 textureColor = texture(texSampler, inTexCoord);
	outColor = vec4((diffuseLight + textureColor.xyz) + specularLight, 1.0);
}