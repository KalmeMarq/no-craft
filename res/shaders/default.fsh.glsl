#version 330 core

in VSOutput
{
  vec4 color;
} fsInput;

out vec4 outColor;

void main()
{
  outColor = vec4(fsInput.color);
}