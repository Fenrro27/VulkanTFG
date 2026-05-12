/**
 * @file scene_shader.vert
 * @brief Archivo scene_shader.vert
 */
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set=0, binding = 0) uniform TransformInfo {
    mat4 MVP;
    mat4 ViewMatrix;
    mat4 ModelViewMatrix;
} Transform;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 Position;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec2 TexCoord;

void main() 
{
	vec4 n4 = Transform.ModelViewMatrix*vec4(inNormal, 0.0);
	vec4 v4 = Transform.ModelViewMatrix*vec4(inPosition,1.0);
	Normal = vec3(n4);
	Position = vec3(v4);
	TexCoord = inTexCoord;
	gl_Position = Transform.MVP * vec4(inPosition, 1.0);
}
