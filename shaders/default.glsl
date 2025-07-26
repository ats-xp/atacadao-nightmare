@ctype mat4 glm::mat4

@vs vs
layout(binding = 0) uniform vs_params {
  mat4 mvp;
};

layout (location = 0) in vec3 apos;
layout (location = 1) in vec4 acolor;

out vec4 color;

void main() {
  color = acolor;
  gl_Position = mvp * vec4(apos, 1.0);
}
@end

@fs fs
out vec4 FragColor;

in vec4 color;

void main() {
  FragColor = color;
}
@end

@program default vs fs
