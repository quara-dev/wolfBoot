# WolfBoot Key Tools

# Get build variables from parent
build_options = get_variable('build_options', {})
c_args = get_variable('c_args', [])
inc_dirs = get_variable('inc_dirs', [])
libwolfboot = get_variable('libwolfboot')

# Key generation tool
keygen = executable('keygen',
  'keygen.c',
  include_directories : inc_dirs,
  c_args : c_args + ['-DWOLFBOOT_KEYGEN'],
  link_with : libwolfboot,
  install : false,
)

# Signing tool
sign_tool = executable('sign',
  'sign.c',
  include_directories : inc_dirs,
  c_args : c_args + ['-DWOLFBOOT_SIGN_TOOL'],
  link_with : libwolfboot,
  install : false,
)

# Export keytools executables to parent scope
set_variable('keytools_exe', [keygen, sign_tool])

# OTP keystore tools
if build_options.get('flash_otp_keystore', false)
  subdir('otp')
endif
