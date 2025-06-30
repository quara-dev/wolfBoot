# Binary assembler tool

bin_assemble_sources = [
  'bin-assemble.c',
]

bin_assemble = executable('bin-assemble',
  bin_assemble_sources,
  include_directories : inc_dirs,
  c_args: ['-Wall', '-g', '-ggdb'],
  build_by_default: true,
  native: true,
  install : false,
)
