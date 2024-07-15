#version 330 core

uniform mat4 uProjection;
uniform mat4 uModelView;
uniform vec4 uPlayer;

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aUV;

out VSOutput
{
  vec4 color;
  vec2 uv;
  float vertexDistance;
} vsOutput;

// Credits: Minecraft
float fog_distance(vec3 pos, int shape) {
    if (shape == 0) {
        return length(pos);
    } else {
        float distXZ = length(pos.xz);
        float distY = abs(pos.y);
        return max(distXZ, distY);
    }
}

void main()
{
  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);
  vsOutput.color = aColor;
  vsOutput.uv = aUV;
  vsOutput.vertexDistance = fog_distance(vec3(aPosition.x - uPlayer.x, aPosition.y, aPosition.z - uPlayer.y), 0);
}