#version 330 core

uniform sampler2D uSampler;

in VSOutput
{
  vec4 color;
  vec2 uv;
} fsInput;

out vec4 outColor;

void main()
{
  outColor = texture(uSampler, fsInput.uv) * vec4(fsInput.color);
}