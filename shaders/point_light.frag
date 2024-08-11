#version 460

#extension GL_KHR_vulkan_glsl : enable

layout (location = 0) in vec2 fragOffset;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform GlobalUbo {
	mat4 projection;
	mat4 view;
    vec4 ambientLightColor;
    vec3 lightPosition;
    vec4 lightColor;
} ubo;

void main()
{
    float dis = sqrt(dot(fragOffset, fragOffset));
    if (dis >= 1.0) discard;
    else outColor = vec4((1 - dis) * vec3(1.0) + dis * ubo.lightColor.xyz, 1.0);
}