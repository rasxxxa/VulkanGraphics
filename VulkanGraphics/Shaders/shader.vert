#version 450 		// Use GLSL 4.5

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;
layout(location = 2) in vec2 tex;

layout(set = 0, binding = 0) uniform UboViewProjection {
	mat4 projection;
	mat4 view;
} uboViewProjection;

// not used anymore because of push constant
layout(set = 0, binding = 1) uniform UboModel {
	mat4 model;
} uboModel;

layout(push_constant) uniform PushModel
{
	mat4 model;
	float hasTex;
	float alpha;
}pushModel;

layout(location = 0) out vec3 fragCol;
layout(location = 1) out vec2 fragTex;
layout(location = 2) out float hasTexX;
layout(location = 3) out float alpha;

void main() {
	gl_Position = uboViewProjection.projection * uboViewProjection.view * pushModel.model * vec4(pos, 1.0);
	
	alpha = pushModel.alpha;
	
	if (pushModel.hasTex > 0.5)
	{
		hasTexX = 1.0;
		fragCol = vec3(1.0, 1.0, 1.0);
	}
	else 
	{
		hasTexX = 0.0;
		fragCol = col;
	}
	
	fragTex = tex;
}