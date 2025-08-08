# lib/meson.build
# WolfSSL cryptographic library build configuration

# Get build options from parent
arch = get_option('arch')
target = get_option('target')
sign = get_option('sign')
hash = get_option('hash')
encrypt = get_option('encrypt')
debug = get_option('wolfboot_debug')

# Base WolfCrypt sources (matching arch.mk defaults)
wolfcrypt_sources = [
  'wolfssl/wolfcrypt/src/sha256.c',
  'wolfssl/wolfcrypt/src/hash.c',
  'wolfssl/wolfcrypt/src/memory.c',
  'wolfssl/wolfcrypt/src/wc_port.c',
  'wolfssl/wolfcrypt/src/wolfmath.c',
  'wolfssl/wolfcrypt/src/asn.c', # From options.mk
  # 'wolfssl/wolfcrypt/src/logging.c', # Debug logging support from arch.mk
]

# Math library sources
if get_option('spmath')
  wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sp_int.c']

  # Add Cortex-M optimized SP math if building for ARM Cortex-M and assembly is enabled
  # Only include when actually cross-compiling for ARM (not on native x86_64)
  if arch == 'ARM' and not get_option('no_asm') and not get_option('no_arm_asm') and meson.is_cross_build()
    cortex_m_targets = (
      get_option('cortex_m0')
      or get_option('cortex_m3')
      or get_option('cortex_m33')
      or get_option('cortex_m7')
    )
    if cortex_m_targets
      wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sp_cortexm.c']
      # Add corresponding compiler flags - these will be handled by parent
      # wolfcrypt_c_args += ['-DWOLFSSL_SP_ASM', '-DWOLFSSL_SP_ARM_CORTEX_M_ASM']
      # if get_option('cortex_m7')
      #   wolfcrypt_c_args += ['-DWOLFSSL_ARM_ARCH=7']
      # elif get_option('cortex_m3')
      #   wolfcrypt_c_args += ['-DWOLFSSL_SP_NO_UMAAL', '-DWOLFSSL_ARM_ARCH=7']
      # endif
    else
      # Default to C implementation for other ARM targets
      wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sp_c32.c']
    endif
  else
    # Use C implementation when assembly is disabled or not cross-compiling
    if arch == 'ARM' or arch == 'AARCH64'
      wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sp_c32.c']
    elif arch == 'x86_64'
      # x86_64 specific SP math handling
      if get_option('no_asm')
        wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sp_c64.c']
      else
        wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sp_x86_64.c']
      endif
    else
      wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sp_c64.c']
    endif
  endif
else
  wolfcrypt_sources += [
    'wolfssl/wolfcrypt/src/integer.c',
    'wolfssl/wolfcrypt/src/tfm.c',
  ]
endif

# Signature algorithm specific sources (matching options.mk)
if sign == 'ECC256' or sign == 'ECC384' or sign == 'ECC521'
  wolfcrypt_sources += ['wolfssl/wolfcrypt/src/ecc.c']
elif sign == 'ED25519'
  wolfcrypt_sources += [
    'wolfssl/wolfcrypt/src/ed25519.c',
    'wolfssl/wolfcrypt/src/ge_low_mem.c',
    'wolfssl/wolfcrypt/src/fe_low_mem.c',
    'wolfssl/wolfcrypt/src/sha512.c',
  ]
elif sign == 'ED448'
  wolfcrypt_sources += [
    'wolfssl/wolfcrypt/src/ed448.c',
    'wolfssl/wolfcrypt/src/ge_low_mem.c',
    'wolfssl/wolfcrypt/src/ge_448.c',
    'wolfssl/wolfcrypt/src/fe_448.c',
    'wolfssl/wolfcrypt/src/fe_low_mem.c',
  ]
  # ED448 adds SHA3 if HASH is not already SHA3 (from options.mk)
  if hash != 'SHA3'
    wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sha3.c']
  endif
elif sign.startswith('RSA')
  wolfcrypt_sources += ['wolfssl/wolfcrypt/src/rsa.c']
elif sign == 'LMS'
  wolfcrypt_sources += [
    'wolfssl/wolfcrypt/src/wc_lms.c',
    'wolfssl/wolfcrypt/src/wc_lms_impl.c',
  ]
elif sign == 'XMSS'
  wolfcrypt_sources += [
    'wolfssl/wolfcrypt/src/wc_xmss.c',
    'wolfssl/wolfcrypt/src/wc_xmss_impl.c',
  ]
elif sign == 'ML_DSA'
  wolfcrypt_sources += ['wolfssl/wolfcrypt/src/dilithium.c']
  # ML_DSA adds SHA3 if HASH is not already SHA3 (from options.mk)
  if hash != 'SHA3'
    wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sha3.c']
  endif
endif

# Hash algorithm specific sources
if hash == 'SHA384'
  wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sha512.c']
elif hash == 'SHA3'
  wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sha3.c']
endif

# Encryption sources
if encrypt
  if get_option('encrypt_with_chacha')
    wolfcrypt_sources += ['wolfssl/wolfcrypt/src/chacha.c']
  elif get_option('encrypt_with_aes128') or get_option('encrypt_with_aes256')
    wolfcrypt_sources += ['wolfssl/wolfcrypt/src/aes.c']
  endif
endif

# TPM sources
tpm_enabled = (
  get_option('wolfboot_tpm_verify')
  or get_option('measured_boot')
  or get_option('wolfboot_tpm_keystore')
  or get_option('wolfboot_tpm_seal')
)

if tpm_enabled
  # Add additional wolfSSL crypto sources needed for TPM to wolfcrypt
  if fs.exists('wolfTPM')
    wolfcrypt_sources += [
      'wolfssl/wolfcrypt/src/aes.c',
      'wolfssl/wolfcrypt/src/hmac.c',
      'wolfssl/wolfcrypt/src/random.c',
    ]
  endif
endif

# Get compiler args and include dirs from parent
wolfcrypt_c_args = c_args  # Use parent's c_args
wolfcrypt_inc_dirs = inc_dirs  # Use parent's inc_dirs

# Add ARM Cortex-M assembly optimization flags (matching CMakeLists.txt behavior)
if arch == 'ARM' and meson.is_cross_build()
  cortex_m_targets = (
    get_option('cortex_m0')
    or get_option('cortex_m3')
    or get_option('cortex_m33')
    or get_option('cortex_m7')
  )
  if cortex_m_targets and not get_option('no_asm') and not get_option('no_arm_asm')
    wolfcrypt_c_args += ['-DWOLFSSL_SP_ASM', '-DWOLFSSL_SP_ARM_CORTEX_M_ASM']
    if get_option('cortex_m7')
      wolfcrypt_c_args += ['-DWOLFSSL_ARM_ARCH=7']
    elif get_option('cortex_m3')
      wolfcrypt_c_args += ['-DWOLFSSL_SP_NO_UMAAL', '-DWOLFSSL_ARM_ARCH=7']
    endif
  endif
endif

wolfcrypt_build_inc_dirs = []

# WolfSSL include directory
wolfssl_inc = include_directories('wolfssl')

# Build WolfCrypt library (cryptographic functions)
libwolfcrypt = static_library(
  'wolfcrypt',
  wolfcrypt_sources,
  include_directories: [wolfcrypt_inc_dirs, wolfcrypt_build_inc_dirs, wolfssl_inc],
  c_args: wolfcrypt_c_args + ['-Wno-unused', '-Wno-array-bounds'],
  install: false,
)

# WolfTPM library (if TPM support is enabled and not a subproject)
libwolftpm = []
wolftpm_sources = []
if tpm_enabled and fs.exists('wolfTPM') and not meson.is_subproject()
  wolftpm_sources = [
    'wolfTPM/src/tpm2.c',
    'wolfTPM/src/tpm2_packet.c',
    'wolfTPM/src/tpm2_tis.c',
    'wolfTPM/src/tpm2_wrap.c',
    'wolfTPM/src/tpm2_param_enc.c',
  ]

  wolftpm_inc = include_directories('wolfTPM')

  libwolftpm = static_library(
    'wolftpm',
    wolftpm_sources,
    include_directories: [wolfcrypt_inc_dirs, wolfcrypt_build_inc_dirs, wolfssl_inc, wolftpm_inc],
    c_args: wolfcrypt_c_args,
    install: false,
  )
endif
