#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
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
  gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
  fragColor = inColor;
  fragTexCoord = inTexCoord;
}

// void main() {
//     gl_Position = vec4(inPosition, 0.0, 1.0);
//     fragColor = inColor;
// }