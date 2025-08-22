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

# Additional critical system flags (matching main build)
if not get_option('vtor')
  c_args += ['-DNO_VTOR']
endif

if get_option('no_mpu')
  c_args += ['-DWOLFBOOT_NO_MPU']
endif

if get_option('ram_code')
  c_args += ['-DRAM_CODE']
endif

if get_option('tzen')
  c_args += ['-DTZEN']
endif

if get_option('wolfcrypt_tz')
  c_args += ['-DWOLFCRYPT_SECURE_MODE']
endif

if get_option('force_32bit')
  c_args += ['-DFORCE_32BIT']
endif

if get_option('big_endian')
  c_args += ['-DBIG_ENDIAN=1']
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
    
    # Use shared Cortex-M CPU detection
    cortex_cpu = cortex_cpu_detected
    
    # Cortex-M specific math optimizations
    if cortex_cpu.contains('cortex-m0')
      c_args += ['-DWOLFSSL_SP_ARM_THUMB_ASM']
      math_sources += ['wolfssl/wolfcrypt/src/sp_armthumb.c']
    elif cortex_cpu.contains('cortex-m3')
      c_args += ['-DWOLFSSL_SP_NO_UMAAL']
      math_sources += ['wolfssl/wolfcrypt/src/sp_cortexm.c']
    else
      # Default Cortex-M4/M7/M33
      math_sources += ['wolfssl/wolfcrypt/src/sp_cortexm.c']
    endif
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
elif sign == 'LMS'
  wolfcrypt_sources += [
    'wolfssl/wolfcrypt/src/wc_lms.c',
    'wolfssl/wolfcrypt/src/wc_lms_impl.c'
  ]
elif sign == 'XMSS'
  wolfcrypt_sources += [
    'wolfssl/wolfcrypt/src/wc_xmss.c',
    'wolfssl/wolfcrypt/src/wc_xmss_impl.c'
  ]
elif sign == 'ML_DSA'
  wolfcrypt_sources += ['wolfssl/wolfcrypt/src/dilithium.c']
endif

# Add secondary signature algorithm sources
sign_secondary = get_option('sign_secondary')
if sign_secondary != ''
  if sign_secondary in ['ECC256', 'ECC384', 'ECC521'] and 'wolfssl/wolfcrypt/src/ecc.c' not in wolfcrypt_sources
    wolfcrypt_sources += ['wolfssl/wolfcrypt/src/ecc.c']
  elif sign_secondary == 'ED25519' and 'wolfssl/wolfcrypt/src/ed25519.c' not in wolfcrypt_sources
    wolfcrypt_sources += [
      'wolfssl/wolfcrypt/src/sha512.c',
      'wolfssl/wolfcrypt/src/ed25519.c',
      'wolfssl/wolfcrypt/src/ge_low_mem.c',
      'wolfssl/wolfcrypt/src/fe_low_mem.c',
    ]
  elif sign_secondary == 'ED448' and 'wolfssl/wolfcrypt/src/ed448.c' not in wolfcrypt_sources
    wolfcrypt_sources += [
      'wolfssl/wolfcrypt/src/ed448.c',
      'wolfssl/wolfcrypt/src/ge_low_mem.c',
      'wolfssl/wolfcrypt/src/ge_448.c',
      'wolfssl/wolfcrypt/src/fe_448.c',
      'wolfssl/wolfcrypt/src/fe_low_mem.c',
    ]
  elif sign_secondary.startswith('RSA') and 'wolfssl/wolfcrypt/src/rsa.c' not in wolfcrypt_sources
    wolfcrypt_sources += ['wolfssl/wolfcrypt/src/rsa.c']
  elif sign_secondary == 'LMS' and 'wolfssl/wolfcrypt/src/wc_lms.c' not in wolfcrypt_sources
    wolfcrypt_sources += [
      'wolfssl/wolfcrypt/src/wc_lms.c',
      'wolfssl/wolfcrypt/src/wc_lms_impl.c'
    ]
  elif sign_secondary == 'XMSS' and 'wolfssl/wolfcrypt/src/wc_xmss.c' not in wolfcrypt_sources
    wolfcrypt_sources += [
      'wolfssl/wolfcrypt/src/wc_xmss.c',
      'wolfssl/wolfcrypt/src/wc_xmss_impl.c'
    ]
  elif sign_secondary == 'ML_DSA' and 'wolfssl/wolfcrypt/src/dilithium.c' not in wolfcrypt_sources
    wolfcrypt_sources += ['wolfssl/wolfcrypt/src/dilithium.c']
  endif
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

# Add PKCS11 sources for TrustZone
if get_option('wolfcrypt_tz_pkcs11')
  wolfcrypt_sources += [
    'wolfssl/wolfcrypt/src/pwdbased.c',
    'wolfssl/wolfcrypt/src/hmac.c',
    'wolfssl/wolfcrypt/src/dh.c',
    'wolfssl/wolfcrypt/src/random.c'
  ]
  # Add AES if not already included from encryption
  if not encrypt_enabled
    wolfcrypt_sources += ['wolfssl/wolfcrypt/src/aes.c']
  endif
endif

# Add TPM sources
if get_option('wolfboot_tpm_verify') or get_option('measured_boot') or get_option('wolfboot_tpm_keystore') or get_option('wolfboot_tpm_seal')
  wolfcrypt_sources += [
    'wolfssl/wolfcrypt/src/aes.c',
    'wolfssl/wolfcrypt/src/hmac.c',
    'wolfssl/wolfcrypt/src/random.c'
  ]
endif

# Add disk lock sources
if get_option('disk_lock')
  wolfcrypt_sources += ['wolfssl/wolfcrypt/src/coding.c']
endif

# Add wolfHSM sources
if get_option('wolfhsm_client')
  wolfcrypt_sources += [
    'wolfssl/wolfcrypt/src/cryptocb.c',
    'wolfssl/wolfcrypt/src/coding.c'
  ]
  
  if sign == 'ML_DSA'
    wolfcrypt_sources += ['wolfssl/wolfcrypt/src/random.c']
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
    include_directories('..', is_system: false),  # Add parent dir for target.h in build
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
    include_directories('..', is_system: false),  # Add parent source directory
  ]
)

# Make the dependency available to parent
set_variable('wolfcrypt_dep', wolfcrypt_dep)
