# wolfCrypt library build configuration
# Extracted from main meson.build for modularity

# Get configuration from parent
fs = import('fs')
inc_dirs = include_directories('..')
c_args = []

# Re-inherit key configuration values from parent
arch = get_option('arch')
sign = get_option('sign')
hash_alg = get_option('hash')
encrypt_enabled = get_option('encrypt')
tpm_enabled = get_option('tpm')

# Math selection flags (inherit from parent logic)
if get_option('spmathall')
  c_args += ['-DWOLFSSL_SP_MATH_ALL']
elif get_option('spmath')
  if arch == 'ARM' and not get_option('no_asm') and not get_option('no_arm_asm')
    c_args += ['-DWOLFSSL_SP_ASM', '-DWOLFSSL_SP_ARM_CORTEX_M_ASM']
  endif
else
  # Fastmath
  c_args += ['-DUSE_FAST_MATH']
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
]

# Math library selection (matches arch.mk logic)
if get_option('spmathall')
  wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sp_int.c']
  if arch == 'ARM' and not get_option('no_asm') and not get_option('no_arm_asm')
    wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sp_cortexm.c']
  else
    wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sp_c32.c']
  endif
elif get_option('spmath')
  # SP Math
  wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sp_int.c']
  if arch == 'ARM' and not get_option('no_asm') and not get_option('no_arm_asm')
    wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sp_cortexm.c']
  else
    wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sp_c32.c']
  endif
else
  # Fastmath
  wolfcrypt_sources += [
    'wolfssl/wolfcrypt/src/integer.c',
    'wolfssl/wolfcrypt/src/tfm.c',
  ]
endif

# Signature algorithm sources
if sign in ['ECC256', 'ECC384', 'ECC521']
  # Elliptic Curve Cryptography
  wolfcrypt_sources += ['wolfssl/wolfcrypt/src/ecc.c']
elif sign == 'ED25519'
  # Edwards-curve Digital Signature Algorithm (25519)
  wolfcrypt_sources += [
    'wolfssl/wolfcrypt/src/sha512.c',
    'wolfssl/wolfcrypt/src/ed25519.c',
    'wolfssl/wolfcrypt/src/ge_low_mem.c',
    'wolfssl/wolfcrypt/src/fe_low_mem.c',
  ]
elif sign == 'ED448'
  # Edwards-curve Digital Signature Algorithm (448)
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
  # RSA signature algorithm
  wolfcrypt_sources += ['wolfssl/wolfcrypt/src/rsa.c']
elif sign == 'LMS'
  # Leighton-Micali Signatures
  wolfcrypt_sources += [
    'wolfssl/wolfcrypt/src/wc_lms.c',
    'wolfssl/wolfcrypt/src/wc_lms_impl.c',
  ]
elif sign == 'XMSS'
  # eXtended Merkle Signature Scheme
  wolfcrypt_sources += [
    'wolfssl/wolfcrypt/src/wc_xmss.c',
    'wolfssl/wolfcrypt/src/wc_xmss_impl.c',
  ]
elif sign == 'ML_DSA'
  # Module-Lattice-Based Digital Signature Algorithm (Dilithium)
  wolfcrypt_sources += [
    'wolfssl/wolfcrypt/src/dilithium.c',
  ]
  if hash_alg != 'SHA3'
    wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sha3.c']
  endif
endif

# Hash algorithm sources
if hash_alg == 'SHA384'
  wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sha512.c']
elif hash_alg == 'SHA3'
  wolfcrypt_sources += ['wolfssl/wolfcrypt/src/sha3.c']
endif

# Encryption algorithm sources
if encrypt_enabled
  if get_option('encrypt_with_aes128') or get_option('encrypt_with_aes256')
    # AES encryption (128-bit or 256-bit keys)
    wolfcrypt_sources += ['wolfssl/wolfcrypt/src/aes.c']
  else
    # ChaCha20 stream cipher (default for encryption)
    wolfcrypt_sources += ['wolfssl/wolfcrypt/src/chacha.c']
  endif
endif

# Build the wolfCrypt static library
libwolfcrypt = static_library(
  'wolfcrypt',
  wolfcrypt_sources,
  include_directories: inc_dirs,
  c_args: c_args + ['-Wno-unused'],
  install: false,
)

# Optional WolfTPM library (compile from sources if present)
libwolftpm = []
if tpm_enabled and fs.exists('wolfTPM')
  inc_dirs_tpm = [inc_dirs, include_directories('wolfTPM')]
  libwolftpm = static_library(
    'wolftpm',
    [
      'wolfTPM/src/tpm2.c',
      'wolfTPM/src/tpm2_packet.c',
      'wolfTPM/src/tpm2_tis.c',
      'wolfTPM/src/tpm2_wrap.c',
      'wolfTPM/src/tpm2_param_enc.c',
    ],
    include_directories: inc_dirs_tpm,
    c_args: c_args,
    install: false,
  )
endif

# Export dependency for parent build system
wolfcrypt_dep = declare_dependency(
  include_directories: inc_dirs,
  link_with: [libwolfcrypt],
  compile_args: c_args,
)

# Export wolfTPM library variable for parent to access
# Note: Set a variable that can be checked by parent
wolfcrypt_has_tpm = libwolftpm != []
