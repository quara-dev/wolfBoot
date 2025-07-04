# WolfBoot Key Tools Build - Full Makefile Implementation

# Include directories
keytools_inc = inc_dirs

# Base compiler flags matching Makefile
keytools_cflags = [
  '-Wall',
  '-Wextra', 
  '-Werror',
  '-DWOLFSSL_USER_SETTINGS',
  '-DDELTA_UPDATES'
]

# Debug vs Release flags
if get_option('wolfboot_debug')
  keytools_cflags += [
    '-g',
    '-DDEBUG',
    '-DDEBUG_SIGNTOOL',
    '-DDEBUG_WOLFSSL',
    '-DDEBUG_WOLFSSL_VERBOSE'
  ]
else
  keytools_cflags += ['-O2']
  # Sanitizer flags could be added here if needed
  # keytools_cflags += ['-fsanitize=address']
endif

# ML-DSA configuration
ml_dsa_level = get_option('ml_dsa_level')
keytools_cflags += ['-DML_DSA_LEVEL=@0@'.format(ml_dsa_level)]

# LMS configuration
lms_levels = get_option('lms_levels')
lms_height = get_option('lms_height')
lms_winternitz = get_option('lms_winternitz')
keytools_cflags += [
  '-DLMS_LEVELS=@0@'.format(lms_levels),
  '-DLMS_HEIGHT=@0@'.format(lms_height),
  '-DLMS_WINTERNITZ=@0@'.format(lms_winternitz)
]

# XMSS configuration
xmss_params = get_option('xmss_params')
image_signature_size = get_option('image_signature_size')
keytools_cflags += [
  '-DIMAGE_SIGNATURE_SIZE=@0@'.format(image_signature_size),
  '-DWOLFBOOT_XMSS_PARAMS="@0@"'.format(xmss_params)
]

# Universal keystore support
if get_option('wolfboot_universal_keystore')
  keytools_cflags += ['-DWOLFBOOT_UNIVERSAL_KEYSTORE']
endif

# Renesas key support (environment variable based like original Makefile)
# This matches the Makefile logic: ifneq ($(RENESAS_KEY),)
renesas_key_env = run_command('printenv', 'RENESAS_KEY', check: false).stdout().strip()
if renesas_key_env != ''
  renesas_key_num = renesas_key_env.to_int()
  keytools_cflags += ['-DRENESAS_KEY=@0@'.format(renesas_key_num)]
  
  if renesas_key_num == 1
    keytools_cflags += ['-DWOLFBOOT_RENESAS_SCEPROTECT']
  elif renesas_key_num == 2
    keytools_cflags += ['-DWOLFBOOT_RENESAS_TSIP']
  elif renesas_key_num == 3
    keytools_cflags += ['-DWOLFBOOT_RENESAS_RSIP']
  endif
endif



# Combine all sources
all_sources = wolfssl_sources + wolfboot_delta_sources

# Key generation tool
keygen = executable('keygen',
  ['keygen.c'] + all_sources,
  include_directories : keytools_inc,
  c_args : keytools_cflags,
  install : false,
  native: true,
  build_by_default : true,
)

# Signing tool  
sign_tool = executable('sign',
  ['sign.c'] + all_sources,
  include_directories : keytools_inc,
  c_args : keytools_cflags,
  install : false,
  native: true,
  build_by_default : true,
)

# Debug target alias
if get_option('wolfboot_debug')
  message('Building keytools in debug mode')
else
  message('Building keytools in release mode')
endif

message('Built keytools with the following configuration:')
message('  - ML-DSA Level: @0@'.format(ml_dsa_level))
message('  - LMS Levels: @0@, Height: @1@, Winternitz: @2@'.format(lms_levels, lms_height, lms_winternitz))
message('  - XMSS Params: @0@'.format(xmss_params))
message('  - Image Signature Size: @0@'.format(image_signature_size))
if get_option('wolfboot_universal_keystore')
  message('  - Universal Keystore: enabled')
endif
if renesas_key_env != ''
  message('  - Renesas Key Type: @0@'.format(renesas_key_env))
endif
