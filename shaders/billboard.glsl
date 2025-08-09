//
// Billboard Shader
//
// TODO Adicionar uniform para alterar o tamanho do sprite

@ctype mat4 glm::mat4
@ctype vec3 glm::vec3

@vs vs_bb
layout(binding = 0) uniform vs_billboard_params {
  mat4 proj, view, model;
  vec3 center;
};

layout (location = 0) in vec3 apos;
// layout (location = 1) in vec3 anormal;
layout (location = 1) in vec2 atex_coords;

out vec2 tex_coords;

void main() {
  vec3 cam_right_ws = vec3(view[0][0], view[1][0], view[2][0]);
  vec3 cam_up_ws = vec3(view[0][1], view[1][1], view[2][1]);

  vec3 billboard_center = center;
  vec2 billboard_size = vec2(2, 2);

  vec3 vertex_pos_ws = 
    billboard_center
    + cam_right_ws * apos.x * billboard_size.x
    + cam_up_ws * apos.y * billboard_size.y;

  tex_coords = atex_coords;
  gl_Position = proj * view *  vec4(vertex_pos_ws, 1.0);
}
@end

@fs fs_bb
layout(binding = 0) uniform texture2D tex;
layout(binding = 0) uniform sampler smp;

in vec2 tex_coords;

out vec4 FragColor;

void main() {
  vec4 tex_color = texture(sampler2D(tex, smp), tex_coords);

  if (tex_color.a < 0.1)
    discard;

  FragColor = tex_color;
}
@end

@program billboard vs_bb fs_bb
