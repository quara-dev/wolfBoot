# OTP Keystore Tools

# OTP keystore generator
otp_keystore_gen = executable('otp-keystore-gen',
  'otp-keystore-gen.c',
  include_directories : inc_dirs,
  c_args : c_args,
  install : false,
)

# OTP keystore primer
otp_keystore_primer = executable('otp-keystore-primer',
  ['otp-keystore-primer.c', 'startup.c'],
  include_directories : inc_dirs,
  c_args : c_args,
  install : false,
)

# Generate OTP keystore primer binary
otp_keystore_primer_bin = custom_target('otp-keystore-primer.bin',
  input : otp_keystore_primer,
  output : 'otp-keystore-primer.bin',
  command : [find_program('objcopy'), '-O', 'binary', '@INPUT@', '@OUTPUT@'],
  install : false,
)
