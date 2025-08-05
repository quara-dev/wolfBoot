project(
  'wolfboot',
  'c',
  version: '2.5.0',
  license: 'GPL-2.0+',
  meson_version: '>=0.61.0',
  default_options: {
    'c_std': 'gnu11',
    'warning_level': '2',
    'buildtype': 'release',
    'wrap_mode': 'nodownload',
    'default_library': 'static',
    'b_staticpic': false,
    'b_pie': false,
    'b_lto': false,
  },
)

# Initialize global variables and modules
cc = meson.get_compiler('c')
fs = import('fs')

# Get build options
arch = get_option('arch')
target = get_option('target')
sign = get_option('sign')
hash = get_option('hash')
encrypt = get_option('encrypt')
debug = get_option('wolfboot_debug')

# Base compiler flags
c_args = [
  '-D__WOLFBOOT',
  '-DWOLFSSL_USER_SETTINGS',
  '-DWOLFTPM_USER_SETTINGS',
]

# Warning flags
warning_flags = [
  '-Wall',
  '-Wextra',
  '-Werror',
  '-Wno-array-bounds',
  '-Wno-error=unused-parameter',
]

foreach flag : warning_flags
  if cc.has_argument(flag)
    c_args += flag
  endif
endforeach

# Embedded flags for GCC headless
if get_option('use_gcc_headless')
  embedded_flags = [
    '-ffreestanding',
    '-Wno-main',
    '-Wno-used',
    '-nostartfiles',
    '-ffunction-sections',
    '-fdata-sections',
    '-fomit-frame-pointer',
  ]

  foreach flag : embedded_flags
    if cc.has_argument(flag)
      c_args += flag
    endif
  endforeach
endif

# Debug flags
if debug
  c_args += ['-g', '-DDEBUG=1', '-DWOLFBOOT_DEBUG=1']
endif

# Architecture-specific flags
if arch == 'ARM'
  c_args += ['-DARCH_ARM']
elif arch == 'AARCH64'
  c_args += ['-DARCH_AARCH64']
elif arch == 'x86_64'
  c_args += ['-DARCH_x86_64', '-DFAST_MEMCPY']
elif arch == 'PPC'
  c_args += ['-DARCH_PPC']
elif arch == 'RISCV'
  c_args += ['-DARCH_RISCV']
endif

# Signing algorithm flags
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
elif sign == 'RSA2048'
  c_args += ['-DWOLFBOOT_SIGN_RSA2048']
elif sign == 'RSA3072'
  c_args += ['-DWOLFBOOT_SIGN_RSA3072']
elif sign == 'RSA4096'
  c_args += ['-DWOLFBOOT_SIGN_RSA4096']
elif sign == 'LMS'
  c_args += ['-DWOLFBOOT_SIGN_LMS']
elif sign == 'XMSS'
  c_args += ['-DWOLFBOOT_SIGN_XMSS']
elif sign == 'ML_DSA'
  c_args += ['-DWOLFBOOT_SIGN_ML_DSA']
elif sign == 'NONE'
  c_args += ['-DWOLFBOOT_NO_SIGN']
endif

# Hash algorithm flags
if hash == 'SHA256'
  c_args += ['-DWOLFBOOT_HASH_SHA256']
elif hash == 'SHA384'
  c_args += ['-DWOLFBOOT_HASH_SHA384']
elif hash == 'SHA3'
  c_args += ['-DWOLFBOOT_HASH_SHA3']
endif

# Encryption flags
if encrypt
  c_args += ['-DEXT_ENCRYPTED=1']
  if get_option('encrypt_with_chacha')
    c_args += ['-DENCRYPT_WITH_CHACHA', '-DHAVE_CHACHA']
  elif get_option('encrypt_with_aes128')
    c_args += ['-DENCRYPT_WITH_AES128', '-DWOLFSSL_AES_128']
  elif get_option('encrypt_with_aes256')
    c_args += ['-DENCRYPT_WITH_AES256', '-DWOLFSSL_AES_256']
  endif
endif

# Feature flags
if get_option('flash_otp_keystore')
  c_args += ['-DFLASH_OTP_KEYSTORE']
endif

if get_option('wolfboot_tpm_verify')
  c_args += ['-DWOLFBOOT_TPM_VERIFY']
endif

if get_option('measured_boot')
  c_args += ['-DWOLFBOOT_MEASURED_BOOT']
endif

if get_option('wolfboot_tpm_keystore')
  c_args += ['-DWOLFBOOT_TPM_KEYSTORE']
endif

if get_option('ext_flash')
  c_args += ['-DEXT_FLASH=1', '-DPART_UPDATE_EXT=1']
endif

if get_option('spi_flash')
  c_args += ['-DSPI_FLASH=1']
endif

if get_option('qspi_flash')
  c_args += ['-DQSPI_FLASH=1']
endif

if get_option('ram_code')
  c_args += ['-DRAM_CODE']
endif

if get_option('allow_downgrade')
  c_args += ['-DALLOW_DOWNGRADE']
endif

if get_option('nvm_flash_writeonce')
  c_args += ['-DNVM_FLASH_WRITEONCE']
endif

if get_option('disable_backup')
  c_args += ['-DDISABLE_BACKUP']
endif

c_args += ['-DCLOCK_SPEED=@0@'.format(get_option('clock_speed'))]
c_args += ['-DSTM32_PLLM=@0@'.format(get_option('stm32_pllm'))]
c_args += ['-DSTM32_PLLN=@0@'.format(get_option('stm32_plln'))]
c_args += ['-DSTM32_PLLP=@0@'.format(get_option('stm32_pllp'))]
c_args += ['-DSTM32_PLLQ=@0@'.format(get_option('stm32_pllq'))]

# Configuration data for target.h
conf_data = configuration_data()
conf_data.set('WOLFBOOT_SECTOR_SIZE', get_option('wolfboot_sector_size'))
conf_data.set(
  'WOLFBOOT_PARTITION_BOOT_ADDRESS',
  get_option('wolfboot_partition_boot_address'),
)
conf_data.set(
  'WOLFBOOT_PARTITION_UPDATE_ADDRESS',
  get_option('wolfboot_partition_update_address'),
)
conf_data.set(
  'WOLFBOOT_PARTITION_SWAP_ADDRESS',
  get_option('wolfboot_partition_swap_address'),
)
conf_data.set('WOLFBOOT_PARTITION_SIZE', get_option('wolfboot_partition_size'))
conf_data.set('WOLFBOOT_DTS_BOOT_ADDRESS', get_option('wolfboot_partition_boot_address'))
conf_data.set(
  'WOLFBOOT_DTS_UPDATE_ADDRESS',
  get_option('wolfboot_partition_update_address'),
)
conf_data.set('WOLFBOOT_LOAD_ADDRESS', get_option('wolfboot_load_address'))
conf_data.set('WOLFBOOT_LOAD_DTS_ADDRESS', get_option('wolfboot_load_address'))

# Generate target.h from template
target_h = configure_file(
  input: 'include/target.h.in',
  output: 'target.h',
  configuration: conf_data,
)

# Math library selection
if get_option('spmath')
  c_args += ['-DWOLFSSL_SP_MATH']
  if get_option('spmathall')
    c_args += ['-DWOLFSSL_SP_MATH_ALL']
  endif
else
  c_args += ['-DUSE_FAST_MATH']
endif

# WolfSSL directory
WolfSSL_inc = include_directories('lib/wolfssl')

# Include directories
local_inc_dirs = include_directories('.', 'include')

inc_dirs = [
  local_inc_dirs,
  WolfSSL_inc,
]

# WolfSSL cryptographic source files (matching Makefile OBJS_REAL)
wolfssl_sources = files(
  'lib/wolfssl/wolfcrypt/src/aes.c',
  'lib/wolfssl/wolfcrypt/src/asn.c',
  'lib/wolfssl/wolfcrypt/src/chacha.c',
  'lib/wolfssl/wolfcrypt/src/coding.c',
  # ML-DSA (Dilithium) support
  'lib/wolfssl/wolfcrypt/src/dilithium.c',
  'lib/wolfssl/wolfcrypt/src/ecc.c',
  'lib/wolfssl/wolfcrypt/src/ed25519.c',
  'lib/wolfssl/wolfcrypt/src/ed448.c',
  'lib/wolfssl/wolfcrypt/src/fe_448.c',
  'lib/wolfssl/wolfcrypt/src/fe_operations.c',
  'lib/wolfssl/wolfcrypt/src/ge_448.c',
  'lib/wolfssl/wolfcrypt/src/ge_operations.c',
  'lib/wolfssl/wolfcrypt/src/hash.c',
  'lib/wolfssl/wolfcrypt/src/logging.c',
  'lib/wolfssl/wolfcrypt/src/memory.c',
  'lib/wolfssl/wolfcrypt/src/random.c',
  'lib/wolfssl/wolfcrypt/src/rsa.c',
  'lib/wolfssl/wolfcrypt/src/sha256.c',
  'lib/wolfssl/wolfcrypt/src/sha3.c',
  'lib/wolfssl/wolfcrypt/src/sha512.c',
  'lib/wolfssl/wolfcrypt/src/sp_c32.c',
  'lib/wolfssl/wolfcrypt/src/sp_c64.c',
  'lib/wolfssl/wolfcrypt/src/sp_int.c',
  'lib/wolfssl/wolfcrypt/src/tfm.c',
  # LMS support
  'lib/wolfssl/wolfcrypt/src/wc_lms.c',
  'lib/wolfssl/wolfcrypt/src/wc_lms_impl.c',
  'lib/wolfssl/wolfcrypt/src/wc_port.c',
  # XMSS support
  'lib/wolfssl/wolfcrypt/src/wc_xmss.c',
  'lib/wolfssl/wolfcrypt/src/wc_xmss_impl.c',
  'lib/wolfssl/wolfcrypt/src/wolfmath.c',
)

# WolfBoot delta update support
wolfboot_delta_sources = files('src/delta.c')

# Additional build directory for generated files
build_inc_dirs = include_directories(['.'], is_system: false)

# Core wolfboot sources
wolfboot_sources = [
  'src/string.c',
  'src/image.c',
  'src/libwolfboot.c',
  'hal/hal.c',
]

# Add loader if not disabled
if not get_option('no_loader')
  wolfboot_sources += ['src/loader.c']
endif

# Add target-specific HAL
if target != 'library'
  hal_file = 'hal/@0@.c'.format(target)
  if fs.exists(hal_file)
    wolfboot_sources += [hal_file]
  else
    warning('HAL file @0@ not found, using skeleton HAL'.format(hal_file))
    wolfboot_sources += ['hal/skeleton.c']
  endif
endif

# WolfCrypt sources
wolfcrypt_sources = [
  'lib/wolfssl/wolfcrypt/src/sha256.c',
  'lib/wolfssl/wolfcrypt/src/hash.c',
  'lib/wolfssl/wolfcrypt/src/memory.c',
  'lib/wolfssl/wolfcrypt/src/wc_port.c',
  'lib/wolfssl/wolfcrypt/src/wolfmath.c',
  'lib/wolfssl/wolfcrypt/src/asn.c',
]

# Math library sources
if get_option('spmath')
  wolfcrypt_sources += ['lib/wolfssl/wolfcrypt/src/sp_int.c']
else
  wolfcrypt_sources += [
    'lib/wolfssl/wolfcrypt/src/integer.c',
    'lib/wolfssl/wolfcrypt/src/tfm.c',
  ]
endif

# Signature algorithm specific sources
if sign == 'ECC256' or sign == 'ECC384' or sign == 'ECC521'
  wolfcrypt_sources += ['lib/wolfssl/wolfcrypt/src/ecc.c']
elif sign == 'ED25519'
  wolfcrypt_sources += [
    'lib/wolfssl/wolfcrypt/src/ed25519.c',
    'lib/wolfssl/wolfcrypt/src/ge_low_mem.c',
    'lib/wolfssl/wolfcrypt/src/fe_low_mem.c',
    'lib/wolfssl/wolfcrypt/src/sha512.c',
  ]
elif sign == 'ED448'
  wolfcrypt_sources += [
    'lib/wolfssl/wolfcrypt/src/ed448.c',
    'lib/wolfssl/wolfcrypt/src/ge_low_mem.c',
    'lib/wolfssl/wolfcrypt/src/ge_448.c',
    'lib/wolfssl/wolfcrypt/src/fe_448.c',
    'lib/wolfssl/wolfcrypt/src/fe_low_mem.c',
    'lib/wolfssl/wolfcrypt/src/sha3.c',
  ]
elif sign.startswith('RSA')
  wolfcrypt_sources += ['lib/wolfssl/wolfcrypt/src/rsa.c']
elif sign == 'LMS'
  wolfcrypt_sources += [
    'lib/wolfssl/wolfcrypt/src/wc_lms.c',
    'lib/wolfssl/wolfcrypt/src/wc_lms_impl.c',
  ]
elif sign == 'XMSS'
  wolfcrypt_sources += [
    'lib/wolfssl/wolfcrypt/src/wc_xmss.c',
    'lib/wolfssl/wolfcrypt/src/wc_xmss_impl.c',
  ]
elif sign == 'ML_DSA'
  wolfcrypt_sources += [
    'lib/wolfssl/wolfcrypt/src/dilithium.c',
    'lib/wolfssl/wolfcrypt/src/sha3.c',
  ]
endif

# Hash algorithm specific sources
if hash == 'SHA384'
  wolfcrypt_sources += ['lib/wolfssl/wolfcrypt/src/sha512.c']
elif hash == 'SHA3'
  wolfcrypt_sources += ['lib/wolfssl/wolfcrypt/src/sha3.c']
endif

# Encryption sources
if encrypt
  if get_option('encrypt_with_chacha')
    wolfcrypt_sources += ['lib/wolfssl/wolfcrypt/src/chacha.c']
  elif get_option('encrypt_with_aes128') or get_option('encrypt_with_aes256')
    wolfcrypt_sources += ['lib/wolfssl/wolfcrypt/src/aes.c']
  endif
endif

# Flash driver sources
if get_option('spi_flash')
  wolfboot_sources += ['src/spi_flash.c']
endif

if get_option('qspi_flash')
  wolfboot_sources += ['src/qspi_flash.c']
endif

if get_option('uart_flash')
  wolfboot_sources += ['src/uart_flash.c']
endif

# TPM sources
tpm_enabled = (
  get_option('wolfboot_tpm_verify')
  or get_option('measured_boot')
  or get_option('wolfboot_tpm_keystore')
  or get_option('wolfboot_tpm_seal')
)

if tpm_enabled
  wolfboot_sources += ['src/tpm.c']
  c_args += ['-DWOLFBOOT_TPM', '-DWOLFTPM_SMALL_STACK', '-DWOLFTPM_AUTODETECT']

  # Add wolfTPM sources if available
  if fs.exists('lib/wolfTPM')
    inc_dirs = include_directories(inc_dirs, 'lib/wolfTPM')
    wolfboot_sources += [
      'lib/wolfTPM/src/tpm2.c',
      'lib/wolfTPM/src/tpm2_packet.c',
      'lib/wolfTPM/src/tpm2_tis.c',
      'lib/wolfTPM/src/tpm2_wrap.c',
      'lib/wolfTPM/src/tpm2_param_enc.c',
    ]
    wolfcrypt_sources += [
      'lib/wolfssl/wolfcrypt/src/aes.c',
      'lib/wolfssl/wolfcrypt/src/hmac.c',
      'lib/wolfssl/wolfcrypt/src/random.c',
    ]
  endif
endif

# Delta update sources
if get_option('delta_updates')
  wolfboot_sources += ['src/delta.c']
  c_args += ['-DDELTA_UPDATES']
endif

# Subprojects - Build tools separately like Makefile
# Only build keytools and bin-assemble when explicitly requested
build_keytools = get_option('build_keytools')
build_bin_assemble = get_option('build_bin_assemble')

# Create separate targets for tools (like Makefile)
keytools_target = []
bin_assemble_target = []
otp_target = []

if build_keytools
  subdir('tools/keytools')
  # Get the keytools from the subdirectory
  keytools_target = get_variable('keytools_executables', [])

  # Add OTP tools if flash_otp_keystore is enabled (matching Makefile logic)
  if get_option('flash_otp_keystore')
    subdir('tools/keytools/otp')
    otp_target = get_variable('otp_tools', [])
  endif
endif

if build_bin_assemble
  subdir('tools/bin-assemble')
  # Get the bin-assemble tool from the subdirectory
  bin_assemble_target = get_variable('bin_assemble_executable', [])
endif

# Add a custom target for keytools that matches Makefile behavior
all_tools = keytools_target + otp_target

keytools_custom = custom_target(
  'keytools',
  output: 'keytools.stamp',
  command: [find_program('touch'), '@OUTPUT@'],
  depends: all_tools,
  build_by_default: false,
  console: true,
)

# Add custom target for OTP tools specifically (matching Makefile otp target)
if get_option('flash_otp_keystore') and build_keytools
  otp_custom = custom_target(
    'otp',
    output: 'otp.stamp',
    command: [find_program('touch'), '@OUTPUT@'],
    depends: otp_target,
    build_by_default: false,
    console: true,
  )
endif

keystore_source = []
# Add keystore sources based on signing configuration
if sign != 'NONE'
  if get_option('flash_otp_keystore')
    keystore_source += ['src/flash_otp_keystore.c']
  else
    keystore_source += ['src/keystore.c']
  endif
endif

# All sources combined
all_sources = wolfboot_sources + wolfcrypt_sources

# Main wolfboot library
libwolfboot = static_library(
  'wolfboot',
  all_sources,
  include_directories: [inc_dirs, build_inc_dirs],
  c_args: c_args,
  install: false,
)

# Create dependency for other projects (needed by subprojects)
wolfboot_dep = declare_dependency(
  include_directories: inc_dirs,
  link_with: libwolfboot,
  compile_args: c_args,
)

# Main executable (if not library target)
if target != 'library'
  # Try to find a linker script

  linker_file = '@0@.ld'.format(target)
  link_args = []
  if (fs.exists('hal/' + linker_file))
    configure_file(
      copy: true,
      input: 'hal/' + linker_file,
      output: linker_file,
    )
  endif

  # Configuration data for target.h
  linker_conf_data = configuration_data()
  linker_conf_data.set('WOLFBOOT_PARTITION_SIZE', get_option('wolfboot_sector_size'))
  linker_conf_data.set(
    'WOLFBOOT_PARTITION_BOOT_ADDRESS',
    get_option('wolfboot_partition_boot_address'),
  )
  linker_conf_data.set(
    'WOLFBOOT_PARTITION_UPDATE_ADDRESS',
    get_option('wolfboot_partition_update_address'),
  )
  linker_conf_data.set(
    'WOLFBOOT_PARTITION_SWAP_ADDRESS',
    get_option('wolfboot_partition_swap_address'),
  )
  linker_conf_data.set('BOOTLOADER_PARTITION_SIZE', get_option('wolfboot_partition_size'))
  linker_conf_data.set('WOLFBOOT_DTS_BOOT_ADDRESS', get_option('wolfboot_partition_boot_address'))
  linker_conf_data.set(
    'WOLFBOOT_DTS_UPDATE_ADDRESS',
    get_option('wolfboot_partition_update_address'),
  )
  linker_conf_data.set('WOLFBOOT_LOAD_ADDRESS', get_option('wolfboot_load_address'))
  linker_conf_data.set('WOLFBOOT_LOAD_DTS_ADDRESS', get_option('wolfboot_load_address'))

  linker_script = configure_file(
    input: 'hal/@0@.ld'.format(target),
    output: '@0@.ld'.format(target),
    configuration: linker_conf_data,
  )

  link_args = [
    '-L' + meson.current_build_dir(),
    '-T' + linker_file,
  ]

  if get_option('use_gcc_headless')
    link_args += [
      '-Wl,--gc-sections',
      '-Wl,-Map=wolfboot.map',
      '-ffreestanding',
      '-nostartfiles',
    ]
  endif

  wolfboot_exe = executable(
    'wolfboot',
    [all_sources, keystore_source],
    include_directories: [inc_dirs, build_inc_dirs],
    c_args: c_args,
    link_args: link_args,
    install: false,
    name_suffix: 'elf',
  )

  # Binary generation
  # objcopy = find_program('objcopy', required: false)
  # if objcopy.found()
  #   wolfboot_bin = custom_target('wolfboot.bin',
  #     input : wolfboot_exe,
  #     output : 'wolfboot.bin',
  #     command : [objcopy, '-O', 'binary', '@INPUT@', '@OUTPUT@'],
  #     install : false,
  #     build_by_default : true,
  #   )

  #   wolfboot_hex = custom_target('wolfboot.hex',
  #     input : wolfboot_exe,
  #     output : 'wolfboot.hex',
  #     command : [objcopy, '-O', 'ihex', '@INPUT@', '@OUTPUT@'],
  #     install : false,
  #   )
  # endif
endif

# Summary
summary(
  {
    'Architecture': arch,
    'Target': target,
    'Signing': sign,
    'Hash': hash,
    'Debug': debug,
    'Encryption': encrypt,
    'TPM Support': tpm_enabled,
    'Build Keytools': build_keytools,
    'Build bin-assemble': build_bin_assemble,
  },
  section: 'WolfBoot Configuration',
)

# Print usage information about separate tool compilation
if not build_keytools
  message(
    '''
NOTE: Keytools are not built by default in this meson build.
To build keytools separately (matching Makefile behavior), use:
  meson configure -Dbuild_keytools=true
  meson compile keytools

Or build directly with:
  meson setup builddir -Dbuild_keytools=true
  meson compile -C builddir keytools
  ''',
  )
endif
