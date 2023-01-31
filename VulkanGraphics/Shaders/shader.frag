#version 450

layout(location = 0) in vec2 fragTex;
layout(location = 1) in float alpha;

layout(set = 2, binding = 0) uniform sampler2D textureSampler;

layout(location = 0) out vec4 outColour; 	// Final output colour (must also have location

void main() {
		outColour = texture (textureSampler, fragTex);
}