fs = import('fs')
# OTP keystore tools

inc = [include_directories('../../..')]

cflags = ['-Wall', '-Wextra']

otp_keystore_gen = executable('otp-keystore-gen',
  ['otp-keystore-gen.c', '../../../src/keystore.c'],
  include_directories: inc,
  c_args: cflags + ['-DFLASH_OTP_KEYSTORE'],
  native: true,
  install: false,
)

# Primer (ELF) if sources exist
primer_srcs = []
if fs.exists('otp-keystore-primer.c')
  primer_srcs += ['otp-keystore-primer.c']
endif
if fs.exists('startup.c')
  primer_srcs += ['startup.c']
endif

if primer_srcs.length() > 0
  primer = executable('otp-keystore-primer',
    primer_srcs,
    include_directories: inc,
    c_args: cflags + ['-DOTP_KEYSTORE_PRIMER'],
    name_suffix: 'elf',
    install: false,
  )

  objcopy_name = meson.get_external_property('objcopy', get_option('objcopy_prog'))
  objcopy_prog = find_program(objcopy_name, required: true)

  custom_target('otp-keystore-primer.bin',
    input: primer,
    output: 'otp-keystore-primer.bin',
    command: [objcopy_prog, '-O', 'binary', '@INPUT@', '@OUTPUT@'],
  )
endif
