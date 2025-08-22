# wolfCrypt library build configuration
# Extracted from main meson.build for modularity

# Get configuration from parent
fs = import('fs')

# Re-inherit key configuration values from parent
arch = get_option('arch')
sign = get_option('sign')
hash_alg = get_option('hash')
encrypt_enabled = get_option('encrypt')

# Base compiler flags for wolfcrypt
c_args = [
  '-D__WOLFBOOT',
  '-DWOLFSSL_USER_SETTINGS',
  '-DWOLFTPM_USER_SETTINGS',
  '-Wall',
  '-Wextra',
  '-Wno-array-bounds',
  '-Wno-main',
]

# Small stack configuration
if get_option('wolfboot_small_stack')
  c_args += ['-DWOLFBOOT_SMALL_STACK', '-DXMALLOC_USER']
endif

# Architecture-specific configuration
if arch == 'ARM'
  c_args += ['-DARCH_ARM']
  if get_option('use_gcc_headless')
    c_args += [
      '-ffreestanding',
      '-ffunction-sections',
      '-fdata-sections',
      '-fomit-frame-pointer',
    ]
  endif
elif arch == 'AARCH64'
  c_args += ['-DARCH_AARCH64']
elif arch == 'x86_64'
  c_args += ['-DARCH_x86_64', '-DFAST_MEMCPY']
endif

# Target-specific configuration
flash_offset = get_option('arch_flash_offset')
c_args += ['-DARCH_FLASH_OFFSET=@0@'.format(flash_offset)]

# Signature algorithm configuration
if sign != 'NONE'
  if sign == 'ECC256'
    c_args += ['-DWOLFBOOT_SIGN_ECC256']
  elif sign == 'ECC384'
    c_args += ['-DWOLFBOOT_SIGN_ECC384']
  elif sign == 'ECC521'
    c_args += ['-DWOLFBOOT_SIGN_ECC521']
  elif sign == 'ED25519'
    c_args += ['-DWOLFBOOT_SIGN_ED25519']
  elif sign == 'ED448'
    c_args += ['-DWOLFBOOT_SIGN_ED448']
  elif sign.startswith('RSA')
    c_args += ['-DWOLFBOOT_SIGN_RSA']
    if sign == 'RSA2048'
      c_args += ['-DWOLFBOOT_SIGN_RSA2048']
    elif sign == 'RSA3072'
      c_args += ['-DWOLFBOOT_SIGN_RSA3072']
    elif sign == 'RSA4096'
      c_args += ['-DWOLFBOOT_SIGN_RSA4096']
    endif
  endif
endif

# Hash algorithm configuration
if hash_alg == 'SHA256'
  c_args += ['-DWOLFBOOT_HASH_SHA256']
elif hash_alg == 'SHA384'
  c_args += ['-DWOLFBOOT_HASH_SHA384']
elif hash_alg == 'SHA3'
  c_args += ['-DWOLFBOOT_HASH_SHA3']
endif

# Image header size
image_header_size = get_option('image_header_size')
if sign != 'NONE'
  if sign == 'ECC256'
    image_signature_size = 64
  elif sign == 'ECC384'
    image_signature_size = 96
  elif sign == 'ECC521'
    image_signature_size = 132
  elif sign == 'ED25519'
    image_signature_size = 64
  elif sign == 'ED448'
    image_signature_size = 114
  elif sign.startswith('RSA')
    if sign == 'RSA2048'
      image_signature_size = 256
    elif sign == 'RSA3072'
      image_signature_size = 384
    elif sign == 'RSA4096'
      image_signature_size = 512
    endif
  endif
else
  image_signature_size = 0
endif
c_args += ['-DIMAGE_HEADER_SIZE=@0@'.format(image_header_size)]
c_args += ['-DIMAGE_SIGNATURE_SIZE=@0@'.format(image_signature_size)]

# Math library configuration (from arch.mk)
if get_option('spmathall')
  c_args += ['-DWOLFSSL_SP_MATH_ALL']
  math_sources = ['wolfssl/wolfcrypt/src/sp_int.c']
  if arch == 'ARM' and not get_option('no_asm') and not get_option('no_arm_asm')
    math_sources += ['wolfssl/wolfcrypt/src/sp_cortexm.c']
  else
    math_sources += ['wolfssl/wolfcrypt/src/sp_c32.c']
  endif
elif get_option('spmath')
  # SP Math enabled, but not all
  math_sources = ['wolfssl/wolfcrypt/src/sp_int.c']
  if arch == 'ARM' and not get_option('no_asm') and not get_option('no_arm_asm')
    c_args += ['-DWOLFSSL_SP_ASM', '-DWOLFSSL_SP_ARM_CORTEX_M_ASM']
    math_sources += ['wolfssl/wolfcrypt/src/sp_cortexm.c']
  else
    math_sources += ['wolfssl/wolfcrypt/src/sp_c32.c']
  endif
else
  # Use fastmath
  c_args += ['-DUSE_FAST_MATH']
  math_sources = [
    'wolfssl/wolfcrypt/src/integer.c',
    'wolfssl/wolfcrypt/src/tfm.c'
  ]
endif

# Core wolfCrypt sources (always included)
wolfcrypt_sources = [
  'wolfssl/wolfcrypt/src/sha256.c',
  'wolfssl/wolfcrypt/src/hash.c',
  'wolfssl/wolfcrypt/src/memory.c',
  'wolfssl/wolfcrypt/src/wc_port.c',
  'wolfssl/wolfcrypt/src/wolfmath.c',
  'wolfssl/wolfcrypt/src/asn.c',
  'wolfssl/wolfcrypt/src/logging.c',
] + math_sources

# Add signature-specific crypto sources
if sign in ['ECC256', 'ECC384', 'ECC521']
  wolfcrypt_sources += ['wolfssl/wolfcrypt/src/ecc.c']
elif sign == 'ED25519'
  wolfcrypt_sources += [
    'wolfssl/wolfcrypt/src/sha512.c',
    'wolfssl/wolfcrypt/src/ed25519.c',
    'wolfssl/wolfcrypt/src/ge_low_mem.c',
    'wolfssl/wolfcrypt/src/fe_low_mem.c',
  ]
elif sign == 'ED448'
  wolfcrypt_sources += [
    'wolfssl/wolfcrypt/src/ed448.c',
    'wolfssl/wolfcrypt/src/ge_low_mem.c',
    'wolfssl/wolfcrypt/src/ge_448.c',
    'wolfssl/wolfcrypt/src/fe_448.c',
    'wolfssl/wolfcrypt/src/fe_low_mem.c',
  ]
  if hash_alg != 'SHA3'
    wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sha3.c']
  endif
elif sign.startswith('RSA')
  wolfcrypt_sources += ['wolfssl/wolfcrypt/src/rsa.c']
endif

# Add hash-specific sources
if hash_alg == 'SHA384'
  wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sha512.c']
elif hash_alg == 'SHA3'
  wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sha3.c']
endif

# Add encryption sources if enabled
if encrypt_enabled
  if get_option('encrypt_with_aes128') or get_option('encrypt_with_aes256')
    wolfcrypt_sources += ['wolfssl/wolfcrypt/src/aes.c']
    if get_option('encrypt_with_aes128')
      c_args += ['-DENCRYPT_WITH_AES128', '-DWOLFSSL_AES_128']
    else
      c_args += ['-DENCRYPT_WITH_AES256', '-DWOLFSSL_AES_256']
    endif
  else
    # Default to ChaCha20
    wolfcrypt_sources += ['wolfssl/wolfcrypt/src/chacha.c']
    c_args += ['-DENCRYPT_WITH_CHACHA']
  endif
  
  encrypt_cache = get_option('encrypt_cache')
  if encrypt_cache > 0
    c_args += ['-DENCRYPT_CACHE=@0@'.format(encrypt_cache)]
  endif
endif

# Build wolfcrypt library
libwolfcrypt = static_library(
  'wolfcrypt',
  wolfcrypt_sources,
  include_directories: [
    include_directories('.'),
    include_directories('../include'),
    include_directories('wolfssl'),
  ],
  c_args: c_args + ['-Wno-unused'],
  install: false,
)

# Export the library for use by parent
wolfcrypt_dep = declare_dependency(
  link_with: libwolfcrypt,
  include_directories: [
    include_directories('.'),
    include_directories('wolfssl'),
  ]
)

# Make the dependency available to parent
set_variable('wolfcrypt_dep', wolfcrypt_dep)
