#version 450

layout(set = 0, binding = 3) uniform sampler2D particleTexture;
layout(location = 0) in vec4 inColor; 
layout(location = 1) in float inAngle; // Recibimos el ángulo del Vertex Shader

layout(location = 0) out vec4 outColor;

void main() {
    
// Matriz de rotación 2D
    float s = sin(inAngle);
    float c = cos(inAngle);
    mat2 rot = mat2(c, -s, s, c);

    // Rotar UVs alrededor del centro (0.5, 0.5)
    vec2 uv = gl_PointCoord - 0.5;
    uv = rot * uv;
    uv += 0.5;

    vec4 texColor = texture(particleTexture, uv);
// hacemos que la textura tienda a negro
     vec3 finalRGB = texColor.rgb * inColor.rgb;

    float finalAlpha = texColor.a * inColor.a;

    outColor = vec4(finalRGB, finalAlpha);

    if (outColor.a < 0.001) discard;
}