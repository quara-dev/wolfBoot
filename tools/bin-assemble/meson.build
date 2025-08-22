# Binary assembly tool for creating combined firmware images

bin_assemble_exe = executable(
  'bin-assemble',
  'bin-assemble.c',
  include_directories: [
    include_directories('../../'),
    include_directories('../../include'),
  ],
  c_args: [
    '-D__WOLFBOOT',
    '-Wall',
    '-Wextra',
  ],
  native: true,
  install: false,
)

# Export program for use by parent build
meson.override_find_program('bin-assemble', bin_assemble_exe)
