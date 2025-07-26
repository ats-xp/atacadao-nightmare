@ctype mat4 glm::mat4

@vs vs_shape
layout(binding = 0) uniform vs_params_shape {
  mat4 mvp;
};

layout (location = 0) in vec4 apos;
layout (location = 1) in vec4 acolor;

out vec4 color;

void main() {
  gl_Position = mvp * apos;
  color = acolor;
}
@end

@fs fs_shape
out vec4 FragColor;

in vec4 color;

void main() {
  FragColor = color;
}
@end

@program shape vs_shape fs_shape
