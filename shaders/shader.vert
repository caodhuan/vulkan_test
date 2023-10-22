#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

// vec2 positions[3] = vec2[](
//     vec2(0.0, -0.5),
//     vec2(0.5, 0.5),
//     vec2(-0.5, 0.5)
// );

layout(binding = 0) uniform UniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
}
ubo;

void main() {
  gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);
  fragColor = inColor;
}

// void main() {
//     gl_Position = vec4(inPosition, 0.0, 1.0);
//     fragColor = inColor;
// }