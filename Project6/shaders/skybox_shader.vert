#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set=0, binding = 0) uniform TransformInfo {
    mat4 Inverse;
} ubo;

layout(location = 0) in vec3 VertexPosition;

layout(location = 0) out vec3 Position;

void main()
{
    gl_Position = vec4(VertexPosition,1.0);
    Position = vec3(ubo.Inverse * gl_Position);
    Position = normalize(Position);
}
