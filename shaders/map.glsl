@ctype mat4 glm::mat4

// Map Shader
@vs vs_map
layout(binding = 0) uniform vs_map_params {
  mat4 mvp;
};

layout (location = 0) in vec3 apos;
layout (location = 1) in vec3 anormal;
layout (location = 2) in vec2 atex_coords;
layout (location = 3) in vec2 alightmap_coords;

out vec2 tex_coords;
out vec2 lm_coords;

void main() {
  tex_coords = atex_coords;
  lm_coords = alightmap_coords;
  gl_Position = mvp * vec4(apos, 1.0);
}
@end

@fs fs_map
layout(binding = 0) uniform texture2D tex;
layout(binding = 0) uniform sampler smp;

layout(binding = 1) uniform texture2D tex_lm;
layout(binding = 1) uniform sampler smp_lm;

in vec2 tex_coords;
in vec2 lm_coords;

out vec4 FragColor;

void main() {
  vec4 diffuse = texture(sampler2D(tex, smp), tex_coords);
  vec4 lightmap = texture(sampler2D(tex_lm, smp_lm), lm_coords);
 
  if (diffuse.a < 0.1)
    discard;

  vec3 color = diffuse.rgb * lightmap.rgb;

  FragColor = vec4(color, 1.0);
  // FragColor = diffuse;
}
@end

@program map vs_map fs_map
