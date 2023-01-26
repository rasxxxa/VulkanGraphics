#version 450

layout(location = 0) in vec3 fragCol;
layout(location = 1) in vec2 fragTex;
layout(location = 2) in float hasTex;
layout(location = 3) in float alpha;

layout(set = 1, binding = 0) uniform sampler2D textureSampler;

layout(location = 0) out vec4 outColour; 	// Final output colour (must also have location

void main() {
	if (hasTex > 0.5)
	{
		vec4 color = texture (textureSampler, fragTex);
		outColour = vec4(color.r, color.g, color.b, alpha);
	}
	else 
	{
		outColour = vec4(fragCol, 1.0);
	}
}