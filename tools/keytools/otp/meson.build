# OTP Keystore Tools - Modern Meson 1.3+ Build

# Get variables from parent scope
build_options = get_variable('build_options', {})
c_args = get_variable('c_args', [])
inc_dirs = get_variable('inc_dirs', [])
wolfboot_dep = get_variable('wolfboot_dep')

# OTP keystore generator
otp_keystore_gen_sources = ['otp-keystore-gen.c']
if fs.exists('common.c')
  otp_keystore_gen_sources += ['common.c']
endif

otp_keystore_gen = executable('otp-keystore-gen',
  otp_keystore_gen_sources,
  include_directories : inc_dirs,
  c_args : c_args + ['-DOTP_KEYSTORE_GEN'],
  dependencies : [wolfboot_dep],
  install : false,
)

# OTP keystore primer - requires special handling for embedded targets
primer_sources = ['otp-keystore-primer.c']
if fs.exists('startup.c')
  primer_sources += ['startup.c']
endif

# Check if we need special embedded flags for the primer
primer_c_args = c_args + ['-DOTP_KEYSTORE_PRIMER']
primer_link_args = []

# For embedded targets, we may need special linker configuration
if build_options.get('use_gcc_headless', false)
  # Look for primer-specific linker script
  primer_ld = 'otp-keystore-primer.ld'
  if fs.exists(primer_ld)
    primer_link_args = ['-T', meson.current_source_dir() / primer_ld]
    message('Using OTP primer linker script: @0@'.format(primer_ld))
  endif
endif

otp_keystore_primer = executable('otp-keystore-primer',
  primer_sources,
  include_directories : inc_dirs,
  c_args : primer_c_args,
  link_args : primer_link_args,
  dependencies : [wolfboot_dep],
  install : false,
  name_suffix : 'elf',
)

# Generate binary file from primer ELF
objcopy = find_program('objcopy', required: true)
otp_keystore_primer_bin = custom_target('otp-keystore-primer.bin',
  input : otp_keystore_primer,
  output : 'otp-keystore-primer.bin',
  command : [objcopy, '-O', 'binary', '@INPUT@', '@OUTPUT@'],
  install : false,
  build_by_default : true,
)

# Generate hex file as well
otp_keystore_primer_hex = custom_target('otp-keystore-primer.hex',
  input : otp_keystore_primer,
  output : 'otp-keystore-primer.hex',
  command : [objcopy, '-O', 'ihex', '@INPUT@', '@OUTPUT@'],
  install : false,
)

# Export targets for parent scope
otp_tools = [otp_keystore_gen, otp_keystore_primer]
otp_binaries = [otp_keystore_primer_bin, otp_keystore_primer_hex]

set_variable('otp_tools', otp_tools)
set_variable('otp_binaries', otp_binaries)

# Custom target for generating OTP keystore data
if not meson.is_cross_build()
  gen_otp_keystore = custom_target('generate-otp-keystore',
    output : 'otp_keystore.h',
    command : [otp_keystore_gen, '--output', '@OUTPUT@'],
    build_by_default : false,
  )
endif

message('Built OTP tools: otp-keystore-gen and otp-keystore-primer')
