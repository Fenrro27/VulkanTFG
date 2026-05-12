/**
 * @file particles_shader.frag
 * @brief Archivo particles_shader.frag
 */
#version 450

layout(set = 0, binding = 3) uniform sampler2D particleTexture;
layout(location = 0) in vec4 inColor; 
layout(location = 1) in float inAngle; // Recibimos el ángulo del Vertex Shader

layout(location = 0) out vec4 outColor;

void main() {
    // Definimos un ángulo para el eje Y (puedes pasarlo como 'in' desde el vertex)
    // Aquí usaré el mismo inAngle para demostrar el efecto, pero podrías usar otro.
    float angleY = inAngle; 

    // 1. Centrar UVs
    vec2 uv = gl_PointCoord - 0.5;

    // Simular rotación en Y (Efecto de perspectiva/escala)
    // El coseno reducirá el ancho de la textura simulando que gira hacia el fondo
    uv.x /= cos(angleY); 

    // Matriz de rotación 2D (Eje Z)
    float s = sin(inAngle);
    float c = cos(inAngle);
    mat2 rot = mat2(c, -s, s, c);
    
    uv = rot * uv;
    uv += 0.5;

    if(uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) discard;

    vec4 texColor = texture(particleTexture, uv);
    vec3 finalRGB = texColor.rgb * inColor.rgb;
    float finalAlpha = texColor.a * inColor.a;

    outColor = vec4(finalRGB, finalAlpha);
    if (outColor.a < 0.001) discard;
}