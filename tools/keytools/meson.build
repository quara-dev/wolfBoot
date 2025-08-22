# Key generation and signing tools

# Basic wolfCrypt sources needed for keytools
# Always included sources (like in Makefile)
keytools_wolfcrypt_sources = [
  '../../lib/wolfssl/wolfcrypt/src/asn.c',
  '../../lib/wolfssl/wolfcrypt/src/aes.c',
  '../../lib/wolfssl/wolfcrypt/src/ecc.c',
  '../../lib/wolfssl/wolfcrypt/src/coding.c',
  '../../lib/wolfssl/wolfcrypt/src/chacha.c',
  '../../lib/wolfssl/wolfcrypt/src/ed25519.c',
  '../../lib/wolfssl/wolfcrypt/src/ed448.c',
  '../../lib/wolfssl/wolfcrypt/src/fe_operations.c',
  '../../lib/wolfssl/wolfcrypt/src/ge_operations.c',
  '../../lib/wolfssl/wolfcrypt/src/fe_448.c',
  '../../lib/wolfssl/wolfcrypt/src/ge_448.c',
  '../../lib/wolfssl/wolfcrypt/src/hash.c',
  '../../lib/wolfssl/wolfcrypt/src/logging.c',
  '../../lib/wolfssl/wolfcrypt/src/memory.c',
  '../../lib/wolfssl/wolfcrypt/src/random.c',
  '../../lib/wolfssl/wolfcrypt/src/integer.c',
  '../../lib/wolfssl/wolfcrypt/src/tfm.c',
  '../../lib/wolfssl/wolfcrypt/src/wolfmath.c',
  '../../lib/wolfssl/wolfcrypt/src/wc_port.c',
  '../../lib/wolfssl/wolfcrypt/src/sha256.c',
  '../../lib/wolfssl/wolfcrypt/src/sha512.c',
  '../../lib/wolfssl/wolfcrypt/src/sha3.c',
  '../../lib/wolfssl/wolfcrypt/src/signature.c',
  '../../lib/wolfssl/wolfcrypt/src/rsa.c',
  '../../lib/wolfssl/wolfcrypt/src/sp_int.c',
  '../../lib/wolfssl/wolfcrypt/src/sp_c32.c',
  '../../lib/wolfssl/wolfcrypt/src/sp_c64.c',
  '../../src/delta.c',
  '../../lib/wolfssl/wolfcrypt/src/wc_lms.c',
  '../../lib/wolfssl/wolfcrypt/src/wc_lms_impl.c',
  '../../lib/wolfssl/wolfcrypt/src/wc_xmss.c',
  '../../lib/wolfssl/wolfcrypt/src/wc_xmss_impl.c',
  '../../lib/wolfssl/wolfcrypt/src/dilithium.c',
]

# Signature algorithm sources
sign = get_option('sign')
if sign in ['ECC256', 'ECC384', 'ECC521']
  keytools_wolfcrypt_sources += ['../../lib/wolfssl/wolfcrypt/src/ecc.c']
elif sign == 'ED25519'
  keytools_wolfcrypt_sources += [
    '../../lib/wolfssl/wolfcrypt/src/sha512.c',
    '../../lib/wolfssl/wolfcrypt/src/ed25519.c',
    '../../lib/wolfssl/wolfcrypt/src/ge_operations.c',
    '../../lib/wolfssl/wolfcrypt/src/ge_low_mem.c',
    '../../lib/wolfssl/wolfcrypt/src/fe_low_mem.c',
  ]
elif sign == 'ED448'
  keytools_wolfcrypt_sources += [
    '../../lib/wolfssl/wolfcrypt/src/ed448.c',
    '../../lib/wolfssl/wolfcrypt/src/ge_operations.c',
    '../../lib/wolfssl/wolfcrypt/src/ge_low_mem.c',
    '../../lib/wolfssl/wolfcrypt/src/ge_448.c',
    '../../lib/wolfssl/wolfcrypt/src/fe_448.c',
    '../../lib/wolfssl/wolfcrypt/src/fe_low_mem.c',
    '../../lib/wolfssl/wolfcrypt/src/sha3.c',
  ]
elif sign.startswith('RSA')
  keytools_wolfcrypt_sources += ['../../lib/wolfssl/wolfcrypt/src/rsa.c']
endif

# Hash algorithm sources
hash_alg = get_option('hash')
if hash_alg == 'SHA384'
  keytools_wolfcrypt_sources += ['../../lib/wolfssl/wolfcrypt/src/sha512.c']
elif hash_alg == 'SHA3'
  keytools_wolfcrypt_sources += ['../../lib/wolfssl/wolfcrypt/src/sha3.c']
endif

# Include directories for keytools
keytools_inc_dirs = [
  include_directories('../../'),
  include_directories('../../include'),
  include_directories('../../lib/wolfssl'),
]

# Compiler flags for keytools (native compilation)
# Build keytools (native compilation for host)
# Get hash algorithm from parent options
hash_alg = get_option('hash')
hash_flag = ''
if hash_alg == 'SHA256'
  hash_flag = '-DWOLFBOOT_HASH_SHA256'
elif hash_alg == 'SHA384'
  hash_flag = '-DWOLFBOOT_HASH_SHA384'
elif hash_alg == 'SHA3'
  hash_flag = '-DWOLFBOOT_HASH_SHA3_384'
endif

keytools_c_args = [
  '-DWOLFSSL_USER_SETTINGS',
  '-DUSE_FAST_MATH',
  '-DHAVE_FFDHE_2048',
  hash_flag,
  '-DDELTA_UPDATES',
  '-DML_DSA_LEVEL=2',
  '-DLMS_LEVELS=1',
  '-DLMS_HEIGHT=10',
  '-DLMS_WINTERNITZ=8',
  '-DWOLFBOOT_XMSS_PARAMS="XMSS-SHA2_10_256"'
]

# Build keygen tool
keygen_sources = [
  'keygen.c',
] + keytools_wolfcrypt_sources

keygen_exe = executable(
  'keygen',
  keygen_sources,
  include_directories: keytools_inc_dirs,
  c_args: keytools_c_args,
  native: true,
  install: false,
)

# Build sign tool
sign_sources = [
  'sign.c',
] + keytools_wolfcrypt_sources

sign_exe = executable(
  'sign',
  sign_sources,
  include_directories: keytools_inc_dirs,
  c_args: keytools_c_args,
  native: true,
  install: false,
)

# Export programs for use by parent build
meson.override_find_program('keygen', keygen_exe)
meson.override_find_program('sign', sign_exe)
