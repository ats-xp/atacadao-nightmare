project(
  'atacadao',
  ['cpp', 'c'],
  meson_version : '>= 1.3.0',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20'],
)

cc = meson.get_compiler('cpp')

# TODO refatorar isso
sokol_shdc = find_program('sokol-shdc', 'vendor/sokol-tools-bin/bin/linux', required: true)

if sokol_shdc.found()

default_sh = custom_target(
  'default.glsl.h',
  input : 'shaders/default.glsl',
  output : 'default.glsl.h',
  command : [
    sokol_shdc,
    '--input', '@INPUT@',
    '--output', '@OUTPUT@',
    '--slang', 'glsl410:glsl300es',
    ]
  )

billboard_shd = custom_target(
  'billboard.glsl.h',
  input : 'shaders/billboard.glsl',
  output : 'billboard.glsl.h',
  command : [
    sokol_shdc,
    '--input', '@INPUT@',
    '--output', '@OUTPUT@',
    '--slang', 'glsl410:glsl300es',
    ]
  )

shape_sh = custom_target(
  'shape.glsl.h',
  input : 'shaders/shape.glsl',
  output : 'shape.glsl.h',
  command : [
    sokol_shdc,
    '--input', '@INPUT@',
    '--output', '@OUTPUT@',
    '--slang', 'glsl410:glsl300es',
    ]
  )

endif

glm_lib_dir = meson.current_source_dir() + '/vendor/glm/build/glm'

dependencies = [
  dependency('x11'),
  dependency('xi'),
  dependency('xcursor'),
  dependency('gl'),
  dependency('assimp'),
  cc.find_library('glm', dirs : glm_lib_dir),
]

subdir('source')

src_dir += default_sh
src_dir += billboard_shd
src_dir += shape_sh

inc = []
inc += include_directories('include')
inc += include_directories('vendor/sokol')
inc += include_directories('vendor/sokol/util')
inc += include_directories('vendor/fontstash/src')
inc += include_directories('vendor/base')
inc += include_directories('vendor/glm/glm/')
inc += include_directories('vendor/stb')

exe = executable(
  'atacadao',
  src_dir,
  include_directories : inc,
  dependencies : dependencies,
  install : false,
)
