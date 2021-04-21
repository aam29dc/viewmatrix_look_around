#version 420

uniform mat4 rot;

in vec2 ttc;
in vec4 vcolor;
out vec4 color;

layout (binding = 0) uniform sampler2D img;
layout (binding = 1) uniform sampler2D img2;

void main(void) {
    color = mix(texture(img, ttc), texture(img2, ttc), 0.5);
}