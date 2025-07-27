@ctype mat4 glm::mat4

@vs vs
layout(binding = 0) uniform vs_params {
  mat4 mvp;
};

layout (location = 0) in vec3 apos;
layout (location = 1) in vec4 acolor;
layout (location = 2) in vec3 anormal;
layout (location = 3) in vec2 atex_coords;

out vec4 color;
out vec2 tex_coords;

void main() {
  color = acolor;
  tex_coords = atex_coords;
  gl_Position = mvp * vec4(apos, 1.0);
}
@end

@fs fs
layout(binding = 0) uniform texture2D tex;
layout(binding = 0) uniform sampler smp;

in vec4 color;
in vec2 tex_coords;

out vec4 FragColor;

void main() {
  FragColor = texture(sampler2D(tex, smp), tex_coords);
  // FragColor = color;
}
@end

@program default vs fs
