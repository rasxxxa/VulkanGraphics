#version 450

layout(location = 0) in vec3 fragCol;
layout(location = 1) in float alpha;

layout(location = 0) out vec4 outColour; 	// Final output colour (must also have location

void main() {
		vec4 color = vec4(fragCol, alpha);
		outColour = color;
}