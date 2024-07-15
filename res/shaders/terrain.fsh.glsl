#version 330 core

uniform float uFogStart;
uniform float uFogEnd;
uniform vec4 uFogColor;

uniform sampler2D uSampler;

in VSOutput
{
  vec4 color;
  vec2 uv;
  float vertexDistance;
} fsInput;

// Credits: Minecraft
vec4 linear_fog(vec4 inColor, float vertexDistance, float fogStart, float fogEnd, vec4 fogColor) {
    if (vertexDistance <= fogStart) {
        return inColor;
    }

    float fogValue = vertexDistance < fogEnd ? smoothstep(fogStart, fogEnd, vertexDistance) : 1.0;
    return vec4(mix(inColor.rgb, fogColor.rgb, fogValue * fogColor.a), inColor.a);
}

out vec4 outColor;

void main()
{
    vec4 color = linear_fog(texture(uSampler, fsInput.uv) * vec4(fsInput.color), fsInput.vertexDistance, uFogStart, uFogEnd, uFogColor);

    if (color.a < 0.1) {
        discard;
    }

    outColor = color;
}