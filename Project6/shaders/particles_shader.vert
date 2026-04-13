#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in vec4 inColor;
//layout(location = 3) in float inTTL;
//layout(location = 4) in float inCurrentTTL;
layout(location = 3) in float inSize;
layout(location = 4) in int inActiveTTL;


layout(location = 0) out vec4 outColor;
layout(location = 1) out float outAngle;

layout(set=0, binding = 0) uniform TransformInfo { mat4 MVP; } Transform;

void main() {
    outAngle = fract(sin(float(gl_VertexIndex)) * 43758.5453) * 6.28318;
    if (inActiveTTL == 0) {
        gl_Position = vec4(-2.0, -2.0, -2.0, 1.0);
    } else {
        gl_Position = Transform.MVP * vec4(inPos, 1.0);
        gl_PointSize = inSize * (1000.0 /gl_Position.w); // Correcion del tamaño de la particula por proximidad
        outColor = inColor;
    }
}