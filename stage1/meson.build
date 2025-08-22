srcs = ['loader_stage1.c']
stage1_exe = executable(
  'loader_stage1',
  srcs,
  include_directories: [include_directories('..'), include_directories('../include')],
  c_args: ['-Wall', '-DBUILD_LOADER_STAGE1', '-ffreestanding', '-fno-builtin'],
  link_args: ['-nostdlib', '-Wl,-nostartfiles'],
  install: false,
)

# Export variable used by the root build to optionally copy into wolfboot_stage1.bin
set_variable('stage1', stage1_exe)
set_variable('has_stage1', true)
