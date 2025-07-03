# WolfBoot Key Tools Build

# Use parent includes
parent_inc_dirs = include_directories('../../include', '../../lib/wolfssl', '../..')

# Key generation tool
keygen = executable('keygen',
  'keygen.c',
  include_directories : parent_inc_dirs,
  c_args : ['-DWOLFBOOT_KEYGEN', '-DWOLFSSL_SIGN_TOOL'],
  install : false,
  build_by_default : true,
)

# Signing tool
sign_tool = executable('sign',
  'sign.c',
  include_directories : parent_inc_dirs,
  c_args : ['-DWOLFBOOT_SIGN_TOOL', '-DWOLFSSL_SIGN_TOOL'],
  install : false,
  build_by_default : true,
)

message('Built keytools: keygen, sign')
