project(
  'atacadao',
  ['cpp', 'c'],
  meson_version : '>= 1.3.0',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20'],
)

cc = meson.get_compiler('cpp')

glm_lib_dir = meson.current_source_dir() + '/vendor/glm/build/glm'
physx_lib_dir = meson.current_source_dir() + '/vendor/PhysX/physx/bin/linux.x86_64/checked'

dependencies = [
  dependency('x11'),
  dependency('xi'),
  dependency('xcursor'),
  dependency('gl'),
  dependency('glfw3'),

  dependency('assimp'),

  cc.find_library('glm', dirs : glm_lib_dir),

  cc.find_library('PhysXCharacterKinematic_static_64', dirs : physx_lib_dir),
  cc.find_library('PhysXCommon_static_64', dirs : physx_lib_dir),
  cc.find_library('PhysXCooking_static_64', dirs : physx_lib_dir),
  cc.find_library('PhysXExtensions_static_64', dirs : physx_lib_dir),
  cc.find_library('PhysXFoundation_static_64', dirs : physx_lib_dir),
  cc.find_library('PhysXPvdSDK_static_64', dirs : physx_lib_dir),
  cc.find_library('PhysX_static_64', dirs : physx_lib_dir),
  cc.find_library('PhysXVehicle2_static_64', dirs : physx_lib_dir),
]

inc = []
inc += include_directories('include')
inc += include_directories('vendor/sokol')
inc += include_directories('vendor/sokol/util')
inc += include_directories('vendor/fontstash/src')
inc += include_directories('vendor/base')
inc += include_directories('vendor/glm/glm/')
inc += include_directories('vendor/stb')
inc += include_directories('vendor/PhysX/physx/include')

sokol_shdc = find_program('sokol-shdc', 'vendor/sokol-tools-bin/bin/linux', required: true)
shd = []
shd_names = [
  'default',
  'billboard',
  'shape',
  ]

if sokol_shdc.found()
  foreach s : shd_names
    shd += custom_target(
      s + '.glsl.h',
      input : 'shaders/' + s + '.glsl',
      output : s + '.glsl.h',
      command : [
        sokol_shdc,
        '--input', '@INPUT@',
        '--output', '@OUTPUT@',
        '--slang', 'glsl410:glsl300es',
        ]
      )
  endforeach
endif

subdir('assets/maps')
subdir('source')

src_dir += shd

exe = executable(
  'atacadao',
  src_dir,
  include_directories : inc,
  dependencies : dependencies,
  install : false,
)
