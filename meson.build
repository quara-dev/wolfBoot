project(
  'wolfboot',
  'c',
  version: '2.5.0',
  license: 'GPL-2.0-or-later',
  meson_version: '>=1.3.0',
  default_options: {
    'c_std': 'gnu11',
    'warning_level': '2',
    'buildtype': 'release',
    'default_library': 'static',
    'b_staticpic': false,
    'b_lto': false,
  },
)

# Compiler and filesystem helpers
cc = meson.get_compiler('c')
fs = import('fs')

# Basic configuration from options
arch = get_option('arch')
target = get_option('target')
sign = get_option('sign')
hash_alg = get_option('hash')
encrypt_enabled = get_option('encrypt')

# Include directories
inc_dirs = [
  include_directories('.'),
  include_directories('include'),
  include_directories('lib/wolfssl'),
]

# Base compiler flags
c_args = [
  '-D__WOLFBOOT',
  '-DWOLFSSL_USER_SETTINGS',
  '-DWOLFTPM_USER_SETTINGS',
  '-Wall',
  '-Wextra',
  '-Wno-array-bounds',
  '-Wno-main',
]

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

# wolfBoot partition addresses and sizes
wolfboot_partition_size = get_option('wolfboot_partition_size')
wolfboot_partition_boot_address = get_option('wolfboot_partition_boot_address')
wolfboot_partition_update_address = get_option('wolfboot_partition_update_address')
wolfboot_partition_swap_address = get_option('wolfboot_partition_swap_address')
wolfboot_sector_size = get_option('wolfboot_sector_size')

# Convert hex strings to integers for calculations  
# Use a simple approach: define bootloader partition size directly
c_args += ['-DBOOTLOADER_PARTITION_SIZE=0x20000']  # Default size for STM32H7

# Signature algorithm configuration
if sign != 'NONE'
  if sign == 'ECC256'
    c_args += ['-DWOLFBOOT_SIGN_ECC256']
    image_signature_size = 64
  elif sign == 'ECC384'
    c_args += ['-DWOLFBOOT_SIGN_ECC384']
    image_signature_size = 96
  elif sign == 'ECC521'
    c_args += ['-DWOLFBOOT_SIGN_ECC521']
    image_signature_size = 132
  elif sign == 'ED25519'
    c_args += ['-DWOLFBOOT_SIGN_ED25519']
    image_signature_size = 64
  elif sign == 'ED448'
    c_args += ['-DWOLFBOOT_SIGN_ED448']
    image_signature_size = 114
  elif sign.startswith('RSA')
    c_args += ['-DWOLFBOOT_SIGN_RSA']
    if sign == 'RSA2048'
      c_args += ['-DWOLFBOOT_SIGN_RSA2048']
      image_signature_size = 256
    elif sign == 'RSA3072'
      c_args += ['-DWOLFBOOT_SIGN_RSA3072']
      image_signature_size = 384
    elif sign == 'RSA4096'
      c_args += ['-DWOLFBOOT_SIGN_RSA4096']
      image_signature_size = 512
    endif
  endif
else
  image_signature_size = 0
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
c_args += ['-DIMAGE_HEADER_SIZE=@0@'.format(image_header_size)]
c_args += ['-DIMAGE_SIGNATURE_SIZE=@0@'.format(image_signature_size)]

# Math library configuration (from arch.mk)
if get_option('spmathall')
  c_args += ['-DWOLFSSL_SP_MATH_ALL']
  math_sources = ['lib/wolfssl/wolfcrypt/src/sp_int.c']
  if arch == 'ARM' and not get_option('no_asm') and not get_option('no_arm_asm')
    math_sources += ['lib/wolfssl/wolfcrypt/src/sp_cortexm.c']
  else
    math_sources += ['lib/wolfssl/wolfcrypt/src/sp_c32.c']
  endif
elif get_option('spmath')
  # SP Math enabled, but not all
  math_sources = ['lib/wolfssl/wolfcrypt/src/sp_int.c']
  if arch == 'ARM' and not get_option('no_asm') and not get_option('no_arm_asm')
    c_args += ['-DWOLFSSL_SP_ASM', '-DWOLFSSL_SP_ARM_CORTEX_M_ASM']
    math_sources += ['lib/wolfssl/wolfcrypt/src/sp_cortexm.c']
  else
    math_sources += ['lib/wolfssl/wolfcrypt/src/sp_c32.c']
  endif
else
  # Use fastmath
  c_args += ['-DUSE_FAST_MATH']
  math_sources = [
    'lib/wolfssl/wolfcrypt/src/integer.c',
    'lib/wolfssl/wolfcrypt/src/tfm.c'
  ]
endif

# Core wolfCrypt sources (always included)
wolfcrypt_sources = [
  'lib/wolfssl/wolfcrypt/src/sha256.c',
  'lib/wolfssl/wolfcrypt/src/hash.c',
  'lib/wolfssl/wolfcrypt/src/memory.c',
  'lib/wolfssl/wolfcrypt/src/wc_port.c',
  'lib/wolfssl/wolfcrypt/src/wolfmath.c',
  'lib/wolfssl/wolfcrypt/src/asn.c',
  'lib/wolfssl/wolfcrypt/src/logging.c',
] + math_sources

# Add signature-specific crypto sources
if sign in ['ECC256', 'ECC384', 'ECC521']
  wolfcrypt_sources += ['lib/wolfssl/wolfcrypt/src/ecc.c']
elif sign == 'ED25519'
  wolfcrypt_sources += [
    'lib/wolfssl/wolfcrypt/src/sha512.c',
    'lib/wolfssl/wolfcrypt/src/ed25519.c',
    'lib/wolfssl/wolfcrypt/src/ge_low_mem.c',
    'lib/wolfssl/wolfcrypt/src/fe_low_mem.c',
  ]
elif sign == 'ED448'
  wolfcrypt_sources += [
    'lib/wolfssl/wolfcrypt/src/ed448.c',
    'lib/wolfssl/wolfcrypt/src/ge_low_mem.c',
    'lib/wolfssl/wolfcrypt/src/ge_448.c',
    'lib/wolfssl/wolfcrypt/src/fe_448.c',
    'lib/wolfssl/wolfcrypt/src/fe_low_mem.c',
  ]
  if hash_alg != 'SHA3'
    wolfcrypt_sources += ['lib/wolfssl/wolfcrypt/src/sha3.c']
  endif
elif sign.startswith('RSA')
  wolfcrypt_sources += ['lib/wolfssl/wolfcrypt/src/rsa.c']
endif

# Add hash-specific sources
if hash_alg == 'SHA384'
  wolfcrypt_sources += ['lib/wolfssl/wolfcrypt/src/sha512.c']
elif hash_alg == 'SHA3'
  wolfcrypt_sources += ['lib/wolfssl/wolfcrypt/src/sha3.c']
endif

# Add encryption sources if enabled
if encrypt_enabled
  if get_option('encrypt_with_aes128') or get_option('encrypt_with_aes256')
    wolfcrypt_sources += ['lib/wolfssl/wolfcrypt/src/aes.c']
    if get_option('encrypt_with_aes128')
      c_args += ['-DENCRYPT_WITH_AES128', '-DWOLFSSL_AES_128']
    else
      c_args += ['-DENCRYPT_WITH_AES256', '-DWOLFSSL_AES_256']
    endif
  else
    # Default to ChaCha20
    wolfcrypt_sources += ['lib/wolfssl/wolfcrypt/src/chacha.c']
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
  include_directories: inc_dirs,
  c_args: c_args + ['-Wno-unused'],
  install: false,
)

# Core wolfBoot sources
wolfboot_sources = [
  'src/string.c',
  'src/image.c', 
  'src/libwolfboot.c',
  'src/update_flash.c',
]

# Add architecture-specific boot file for ARM targets
if arch in ['ARM', 'ARM_BE'] or target in ['stm32f4', 'stm32f7', 'stm32h7', 'stm32l0', 'stm32l4', 'stm32l5', 'stm32u5', 'stm32h5', 'stm32wb', 'stm32g0', 'stm32c0', 'nrf52', 'nrf5340', 'kinetis', 'mcxa', 'mcxw', 'lpc', 'same51', 'samr21', 'sama5d3', 'pic32c', 'pic32ck', 'pic32cz', 'psoc6']
  wolfboot_sources += ['src/boot_arm.c']
endif

# Add loader unless disabled
if not get_option('no_loader')
  wolfboot_sources += ['src/loader.c']
endif

# HAL sources
hal_sources = ['hal/hal.c']

# Add target-specific HAL unless building library target
if target != 'library'
  hal_sources += ['hal/@0@.c'.format(target)]
endif

# Flash driver sources based on options
if get_option('spi_flash')
  hal_sources += ['src/spi_flash.c']
  spi_target = get_option('spi_target')
  if fs.exists('hal/spi/spi_drv_@0@.c'.format(spi_target))
    hal_sources += ['hal/spi/spi_drv_@0@.c'.format(spi_target)]
  endif
endif

if get_option('qspi_flash')
  hal_sources += ['src/qspi_flash.c']
endif

if get_option('uart_flash')
  hal_sources += ['src/uart_flash.c']
  uart_target = get_option('uart_target')
  if fs.exists('hal/uart/uart_drv_@0@.c'.format(uart_target))
    hal_sources += ['hal/uart/uart_drv_@0@.c'.format(uart_target)]
  endif
endif

# Build HAL library
libwolfboothal = static_library(
  'wolfboothal',
  hal_sources,
  include_directories: inc_dirs,
  c_args: c_args + ['-Wno-unused-parameter', '-Wno-unused-variable'],
  install: false,
)

# Public key sources
public_key_sources = []
if sign != 'NONE'
  if get_option('flash_otp_keystore')
    public_key_sources += ['src/flash_otp_keystore.c']
  else
    public_key_sources += ['src/keystore.c']
  endif
endif

# Build public key library
libpublic_key = static_library(
  'public_key',
  public_key_sources.length() > 0 ? public_key_sources : ['src/string.c'], # dummy source if empty
  include_directories: inc_dirs,
  c_args: c_args,
  install: false,
)

# Build main wolfboot library
libwolfboot = static_library(
  'wolfboot',
  wolfboot_sources,
  include_directories: inc_dirs,
  c_args: c_args,
  link_with: [libwolfcrypt, libwolfboothal, libpublic_key],
  install: false,
)

# Generate target.h configuration file
target_h_data = configuration_data()
target_h_data.set('WOLFBOOT_SECTOR_SIZE', wolfboot_sector_size)
target_h_data.set('WOLFBOOT_PARTITION_BOOT_ADDRESS', wolfboot_partition_boot_address)
target_h_data.set('WOLFBOOT_PARTITION_UPDATE_ADDRESS', wolfboot_partition_update_address)
target_h_data.set('WOLFBOOT_PARTITION_SWAP_ADDRESS', wolfboot_partition_swap_address)
target_h_data.set('WOLFBOOT_PARTITION_SIZE', wolfboot_partition_size)
target_h_data.set('WOLFBOOT_DTS_BOOT_ADDRESS', wolfboot_partition_boot_address)
target_h_data.set('WOLFBOOT_DTS_UPDATE_ADDRESS', wolfboot_partition_update_address)
target_h_data.set('WOLFBOOT_LOAD_ADDRESS', get_option('wolfboot_origin'))
target_h_data.set('WOLFBOOT_LOAD_DTS_ADDRESS', get_option('wolfboot_origin'))

target_h = configure_file(
  input: 'include/target.h.in',
  output: 'target.h',
  configuration: target_h_data,
  install_dir: get_option('includedir'),
)

# Generate linker script
if target != 'library'
  linker_script_input = 'hal/@0@.ld'.format(target)
  if not fs.exists(linker_script_input)
    error('Linker script @0@ not found'.format(linker_script_input))
  endif
  
  linker_script_data = configuration_data()
  linker_script_data.set('ARCH_FLASH_OFFSET', flash_offset)
  linker_script_data.set('BOOTLOADER_PARTITION_SIZE', '0x20000')  # Default for STM32H7
  linker_script_data.set('WOLFBOOT_ORIGIN', get_option('wolfboot_origin'))
  linker_script_data.set('WOLFBOOT_PARTITION_BOOT_ADDRESS', wolfboot_partition_boot_address)
  linker_script_data.set('WOLFBOOT_PARTITION_UPDATE_ADDRESS', wolfboot_partition_update_address)
  linker_script_data.set('WOLFBOOT_PARTITION_SWAP_ADDRESS', wolfboot_partition_swap_address)
  linker_script_data.set('WOLFBOOT_PARTITION_SIZE', wolfboot_partition_size)
  
  target_ld = configure_file(
    input: linker_script_input,
    output: 'target.ld',
    configuration: linker_script_data,
  )
endif

# Link arguments for executable
link_args = []
if get_option('use_gcc_headless')
  link_args += [
    '-ffreestanding',
    '-nostartfiles',
    '-Wl,-gc-sections',
    '-Wl,-Map=@0@'.format(meson.current_build_dir() / 'wolfboot.map'),
  ]
endif

if target != 'library'
  link_args += ['-T', meson.current_build_dir() / 'target.ld']
endif

# Build main wolfboot executable (unless library target)
if target != 'library'
  wolfboot_elf = executable(
    'wolfboot.elf',
    [],  # No additional sources, everything is in the libraries
    include_directories: inc_dirs,
    c_args: c_args,
    link_args: link_args,
    link_whole: [libwolfboot],
    link_with: [libwolfcrypt, libwolfboothal, libpublic_key],
    install: false,
  )

  # Create binary from ELF
  objcopy_prog = find_program(get_option('objcopy_prog'), required: true)
  
  wolfboot_bin = custom_target(
    'wolfboot.bin',
    input: wolfboot_elf,
    output: 'wolfboot.bin',
    command: [objcopy_prog, '-O', 'binary', '@INPUT@', '@OUTPUT@'],
    install: false,
    build_by_default: true,
  )
endif

# Build tools if requested
if get_option('build_keytools')
  subdir('tools/keytools')
endif

if get_option('build_bin_assemble')
  subdir('tools/bin-assemble')
endif

# Summary
summary({
  'Architecture': arch,
  'Target': target,
  'Signature': sign,
  'Hash': hash_alg,
  'Encryption': encrypt_enabled,
  'SP Math All': get_option('spmathall'),
  'SP Math': get_option('spmath'),
}, section: 'Configuration')

message('wolfBoot configured successfully')
