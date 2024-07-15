#version 330 core

uniform mat4 uProjection;
uniform mat4 uModelView;

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aUV;

out VSOutput
{
  vec4 color;
  vec2 uv;
} vsOutput;

void main()
{
  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);
  vsOutput.color = aColor;
  vsOutput.uv = aUV;
}