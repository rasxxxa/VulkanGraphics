#version 460 		// Use GLSL 4.6

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;

layout(set = 0, binding = 0) uniform UboViewProjection {
	mat4 projection;
	mat4 view;
} uboViewProjection;

struct ObjectData{
	mat4 model;
	vec4 additional;
}; 

//all object matrices
layout(std140,set = 1, binding = 0) readonly buffer ObjectBuffer{   

	ObjectData objects[];
} objectBuffer;


layout(location = 0) out vec3 color;
layout(location = 1) out float alpha;

void main() {

	ObjectData object = objectBuffer.objects[gl_BaseInstance];
	mat4 modelMatrix = object.model;
	mat4 transformMatrix = (uboViewProjection.projection * uboViewProjection.view * modelMatrix);
	gl_Position = transformMatrix * vec4(pos, 1.0f);
	alpha = object.additional[0];
	color = col;
}