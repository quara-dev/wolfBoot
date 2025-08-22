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

# Include cortex CPU detection module
subdir('meson/cortex_detection')

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

# Small stack configuration
if get_option('wolfboot_small_stack')
  c_args += ['-DWOLFBOOT_SMALL_STACK', '-DXMALLOC_USER']
endif

# Critical system configuration flags
if not get_option('vtor')
  c_args += ['-DNO_VTOR']
endif

if get_option('no_mpu')
  c_args += ['-DWOLFBOOT_NO_MPU']
endif

if get_option('ram_code')
  c_args += ['-DRAM_CODE']
endif

if get_option('dualbank_swap')
  c_args += ['-DDUALBANK_SWAP=1']
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

if get_option('flags_home')
  c_args += ['-DFLAGS_HOME=1']
endif

if get_option('flags_invert')
  c_args += ['-DWOLFBOOT_FLAGS_INVERT=1']
endif

wolfboot_version = get_option('wolfboot_version')
if wolfboot_version != 0
  c_args += ['-DWOLFBOOT_VERSION=@0@'.format(wolfboot_version)]
endif

# ARM TrustZone configuration
if get_option('tzen')
  c_args += ['-DTZEN']
endif

if get_option('wolfcrypt_tz')
  c_args += ['-DWOLFCRYPT_SECURE_MODE']
endif

if get_option('wolfcrypt_tz_pkcs11')
  c_args += ['-DSECURE_PKCS11', '-DWOLFSSL_PKCS11_RW_TOKENS']
  c_args += ['-DCK_CALLABLE="__attribute__((cmse_nonsecure_entry))"']
  c_args += ['-DWP11_HASH_PIN_COST=3']
endif

# Hardware acceleration and platform features
if get_option('pka')
  c_args += ['-DPKA=1']
endif

if get_option('psoc6_crypto')
  c_args += ['-DPSOC6_CRYPTO=1']
endif

if get_option('armored')
  c_args += ['-DWOLFBOOT_ARMORED']
endif

if get_option('wolfboot_huge_stack')
  c_args += ['-DWOLFBOOT_HUGE_STACK']
endif

if get_option('no_xip')
  c_args += ['-DNO_XIP']
endif

# Advanced features
if get_option('elf')
  c_args += ['-DWOLFBOOT_ELF']
endif

if get_option('multiboot2')
  c_args += ['-DWOLFBOOT_MULTIBOOT2']
endif

if get_option('linux_payload')
  c_args += ['-DWOLFBOOT_LINUX_PAYLOAD']
endif

if get_option('64bit')
  c_args += ['-DWOLFBOOT_64BIT']
endif

if get_option('wolfboot_universal_keystore')
  c_args += ['-DWOLFBOOT_UNIVERSAL_KEYSTORE']
endif

if get_option('disk_lock')
  c_args += ['-DWOLFBOOT_ATA_DISK_LOCK']
  disk_lock_password = get_option('disk_lock_password')
  if disk_lock_password != ''
    c_args += ['-DWOLFBOOT_ATA_DISK_LOCK_PASSWORD="@0@"'.format(disk_lock_password)]
  endif
endif

if get_option('force_32bit')
  c_args += ['-DFORCE_32BIT']
endif

if get_option('big_endian')
  c_args += ['-DBIG_ENDIAN=1']
endif

if get_option('flash_multi_sector_erase')
  c_args += ['-DWOLFBOOT_FLASH_MULTI_SECTOR_ERASE']
endif

# wolfHSM support
if get_option('wolfhsm_client')
  c_args += ['-DWOLFHSM_CLIENT=1']
  if get_option('wolfhsm_client_local_keys')
    c_args += ['-DWOLFHSM_CLIENT_LOCAL_KEYS=1']
  endif
endif

# Secondary signature support
sign_secondary = get_option('sign_secondary')
if sign_secondary != ''
  c_args += ['-DSIGN_HYBRID', '-DWOLFBOOT_SIGN_SECONDARY_@0@'.format(sign_secondary)]
endif

# LMS/XMSS/ML-DSA parameters
if sign == 'LMS'
  lms_levels = get_option('lms_levels')
  lms_height = get_option('lms_height')
  lms_winternitz = get_option('lms_winternitz')
  c_args += [
    '-DLMS_LEVELS=@0@'.format(lms_levels),
    '-DLMS_HEIGHT=@0@'.format(lms_height),
    '-DLMS_WINTERNITZ=@0@'.format(lms_winternitz)
  ]
endif

if sign == 'XMSS'
  xmss_params = get_option('xmss_params')
  c_args += ['-DWOLFBOOT_XMSS_PARAMS="@0@"'.format(xmss_params)]
endif

if sign == 'ML_DSA'
  ml_dsa_level = get_option('ml_dsa_level')
  c_args += ['-DML_DSA_LEVEL=@0@'.format(ml_dsa_level)]
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
  
  # Use shared Cortex-M CPU detection
  cortex_cpu = cortex_cpu_detected
  
  # Set Cortex-M specific flags based on detected CPU
  if cortex_cpu.contains('cortex-m0')
    c_args += ['-DCORTEX_M0']
    message('Auto-detected Cortex-M0 processor')
  elif cortex_cpu.contains('cortex-m3')
    c_args += ['-DCORTEX_M3']
    message('Auto-detected Cortex-M3 processor')
  elif cortex_cpu.contains('cortex-m7')
    c_args += ['-DCORTEX_M7']
    message('Auto-detected Cortex-M7 processor')
  elif cortex_cpu.contains('cortex-m33') or cortex_cpu.contains('cortex-m35')
    c_args += ['-DCORTEX_M33']
    message('Auto-detected Cortex-M33/M35 processor')
  elif cortex_cpu.contains('cortex-m4')
    c_args += ['-DCORTEX_M4']
    message('Auto-detected Cortex-M4 processor')
  elif cortex_cpu.contains('cortex-m23')
    c_args += ['-DCORTEX_M23']
    message('Auto-detected Cortex-M23 processor')
  elif cortex_cpu != ''
    message('Detected ARM CPU: @0@ (no specific Cortex-M optimizations applied)'.format(cortex_cpu))
  else
    message('ARM architecture detected but no specific Cortex-M CPU identified')
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

# Build wolfcrypt library from lib subdirectory
subdir('lib')
# Get the wolfcrypt dependency from the subdirectory
wolfcrypt_dep = get_variable('wolfcrypt_dep')

# Create additional include directory for generated headers
# This allows subprojects to find generated target.h when available
# Note: The actual target.h will be generated later, but this sets up the include path
generated_inc = declare_dependency(
  include_directories: include_directories('.', is_system: false)
)

# Core wolfBoot sources
wolfboot_sources = [
  'src/string.c',
  'src/image.c', 
  'src/libwolfboot.c',
  'src/update_flash.c',
]

# Add xmalloc if small stack is enabled
if get_option('wolfboot_small_stack')
  wolfboot_sources += ['src/xmalloc.c']
endif

# Add delta updates support
if get_option('delta_updates')
  wolfboot_sources += ['src/delta.c']
endif

# Add ELF loader support
if get_option('elf')
  wolfboot_sources += ['src/elf.c']
endif

# Add multiboot support
if get_option('multiboot2')
  wolfboot_sources += ['src/multiboot.c']
endif

# Add disk lock support
if get_option('disk_lock')
  # Note: This would require additional wolfSSL coding sources
  # wolfboot_sources += ['lib/wolfssl/wolfcrypt/src/coding.c']
endif

# Add PKCS11 and TrustZone sources
if get_option('wolfcrypt_tz_pkcs11')
  wolfboot_sources += [
    'src/pkcs11_store.c',
    'src/pkcs11_callable.c'
  ]
endif

# Add TrustZone callable functions
if get_option('wolfcrypt_tz')
  wolfboot_sources += ['src/wc_callable.c']
endif

# Add TPM support sources
if get_option('wolfboot_tpm_verify') or get_option('measured_boot') or get_option('wolfboot_tpm_keystore') or get_option('wolfboot_tpm_seal')
  wolfboot_sources += ['src/tpm.c']
endif

# Add architecture-specific boot file for ARM targets
if arch in ['ARM', 'ARM_BE'] or target in ['stm32f4', 'stm32f7', 'stm32h7', 'stm32l0', 'stm32l4', 'stm32l5', 'stm32u5', 'stm32h5', 'stm32wb', 'stm32g0', 'stm32c0', 'nrf52', 'nrf5340', 'kinetis', 'mcxa', 'mcxw', 'lpc', 'same51', 'samr21', 'sama5d3', 'pic32c', 'pic32ck', 'pic32cz', 'psoc6']
  wolfboot_sources += ['src/boot_arm.c']
elif arch == 'AARCH64'
  wolfboot_sources += ['src/boot_aarch64.c', 'src/boot_aarch64_start.c']
elif arch == 'x86_64'
  if target == 'x86_64_efi'
    wolfboot_sources += ['src/boot_x86_64.c']
  endif
elif arch == 'RISCV'
  wolfboot_sources += ['src/boot_riscv.c']
elif arch == 'PPC'
  wolfboot_sources += ['src/boot_ppc.c', 'src/boot_ppc_start.c']
elif arch == 'RENESAS_RX'
  wolfboot_sources += ['src/boot_renesas.c', 'src/boot_renesas_start.c']
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

# Add TrustZone specific HAL for STM32 targets
if get_option('tzen') and target.startswith('stm32')
  hal_sources += ['hal/stm32_tz.c']
endif

# Add architecture-specific HAL files
if arch == 'RENESAS_RX'
  hal_sources += ['hal/renesas-rx.c']
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
  c_args: c_args,
  dependencies: generated_inc,  # Include generated headers
  install: false,
)

# Public key sources - will be updated after keystore generation if needed
public_key_sources = []

if sign != 'NONE'
  if get_option('flash_otp_keystore')
    public_key_sources += ['src/flash_otp_keystore.c']
  else
    # keystore.c will be generated by keygen tool
    # Don't add it here - it will be handled in the generation section
  endif
endif

# Collect flash driver sources for libwolfboot (similar to WOLFBOOT_FLASH_SOURCES in CMake)
flash_sources = []
if get_option('spi_flash')
  flash_sources += ['src/spi_flash.c']
  spi_target = get_option('spi_target')
  if fs.exists('hal/spi/spi_drv_@0@.c'.format(spi_target))
    flash_sources += ['hal/spi/spi_drv_@0@.c'.format(spi_target)]
  endif
endif

if get_option('qspi_flash')
  flash_sources += ['src/qspi_flash.c']
endif

if get_option('uart_flash')
  flash_sources += ['src/uart_flash.c']
  uart_target = get_option('uart_target')
  if fs.exists('hal/uart/uart_drv_@0@.c'.format(uart_target))
    flash_sources += ['hal/uart/uart_drv_@0@.c'.format(uart_target)]
  endif
endif

# Libraries will be built after keystore generation to resolve dependencies

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

# Build tools if requested
need_keygen = sign != 'NONE' and get_option('flash_otp_keystore') == false
if get_option('build_keytools') or need_keygen
  subdir('tools/keytools')
endif

if get_option('build_bin_assemble')
  subdir('tools/bin-assemble')
endif

# Generate keystore.c 
if need_keygen
  public_key_path = get_option('public_key_path')
  
  # Convert sign option to keygen flag
  keygen_sign_flag = ''
  if sign == 'ECC256'
    keygen_sign_flag = '--ecc256'
  elif sign == 'ECC384'
    keygen_sign_flag = '--ecc384'
  elif sign == 'ECC521'
    keygen_sign_flag = '--ecc521'
  elif sign == 'ED25519'
    keygen_sign_flag = '--ed25519'
  elif sign == 'ED448'
    keygen_sign_flag = '--ed448'
  elif sign == 'RSA2048'
    keygen_sign_flag = '--rsa2048'
  elif sign == 'RSA3072'
    keygen_sign_flag = '--rsa3072'
  elif sign == 'RSA4096'
    keygen_sign_flag = '--rsa4096'
  elif sign == 'LMS'
    keygen_sign_flag = '--lms'
  elif sign == 'XMSS'
    keygen_sign_flag = '--xmss'
  elif sign == 'ML_DSA'
    keygen_sign_flag = '--ml-dsa'
  endif
  
  if keygen_sign_flag != ''
    if public_key_path != ''
      # Generate keystore.c from external public key
      # Note: keygen tool writes to src/keystore.c by default, so we copy it to build dir
      keystore_c_target = custom_target('keystore_c',
        input: public_key_path,
        output: 'keystore.c',
        command: [
          'sh', '-c',
          '&&'.join([
            'cd @0@'.format(meson.current_source_dir()),
            'mkdir -p src',
            '@0@ --force @1@ -i @2@'.format(keygen_exe.full_path(), keygen_sign_flag, public_key_path),
            'cp src/keystore.c @0@/@1@'.format(meson.current_build_dir(), '@OUTPUT0@')
          ])
        ],
        build_by_default: true,
      )
      
      # Add generated keystore to sources
      public_key_sources += [keystore_c_target]
      message('Generated keystore.c from external public key: @0@'.format(public_key_path))
    else
      # Generate keystore.c from generated private key (standard workflow)
      # For now, we'll create a minimal keystore for SIGN=NONE case
      keystore_c_target = custom_target('keystore_c_empty',
        output: 'keystore.c',
        command: [
          'echo', 
          '// SIGN=NONE\n#include "keystore.h"\n#ifdef WOLFBOOT_NO_SIGN\n#define NUM_PUBKEYS 0\nconst struct keystore_slot PubKeys[NUM_PUBKEYS] = { };\nint keystore_num_pubkeys(void) { return 0; }\n#endif'
        ],
        capture: true,
        build_by_default: true,
      )
      
      # Add generated keystore to sources
      public_key_sources += [keystore_c_target]
      message('Generated empty keystore.c for sign=@0@'.format(sign))
    endif
  else
    error('Unsupported signature algorithm for keystore generation: @0@'.format(sign))
  endif
endif

# Build public key library after keystore generation
library_sources = public_key_sources
if library_sources.length() == 0
  library_sources = ['src/string.c']  # dummy source if empty
endif

libpublic_key = static_library(
  'public_key',
  library_sources,
  include_directories: inc_dirs,
  c_args: c_args,
  dependencies: generated_inc,  # Include generated headers
  install: false,
)

# Build libwolfboot static library (matches CMake libwolfboot)
libwolfboot = static_library(
  'wolfboot',
  ['src/libwolfboot.c'] + flash_sources,
  include_directories: inc_dirs,
  c_args: c_args + ['-Wno-unused'],
  dependencies: [wolfcrypt_dep, generated_inc],  # Include generated headers
  link_with: [libwolfboothal, libpublic_key],
  install: false,
)

# Build main wolfboot implementation library
libwolfboot_main = static_library(
  'wolfboot_main',
  wolfboot_sources,
  include_directories: inc_dirs,
  c_args: c_args,
  dependencies: [wolfcrypt_dep, generated_inc],  # Include generated headers
  install: false,
)

# Build main wolfboot executable (unless library target)
if target != 'library'
  wolfboot_elf = executable(
    'wolfboot.elf',
    [],  # No additional sources, everything is in the libraries
    include_directories: inc_dirs,
    c_args: c_args,
    link_args: link_args,
    link_whole: [libwolfboot_main],
    dependencies: [wolfcrypt_dep, generated_inc],  # Include generated headers
    link_with: [libwolfboothal, libpublic_key],
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

# Declare dependencies for use as subproject
# This makes all wolfBoot libraries available to parent projects
wolfboot_dep = declare_dependency(
  link_with: libwolfboot,
  include_directories: inc_dirs,
  compile_args: c_args,
  dependencies: [wolfcrypt_dep, generated_inc],  # Include generated headers
)

wolfboot_main_dep = declare_dependency(
  link_with: [libwolfboot_main, libwolfboothal, libpublic_key],
  include_directories: inc_dirs,
  compile_args: c_args,
  dependencies: [wolfcrypt_dep, generated_inc],  # Include generated headers
)

wolfboothal_dep = declare_dependency(
  link_with: libwolfboothal,
  include_directories: inc_dirs,
  compile_args: c_args,
  dependencies: generated_inc,  # Include generated headers
)

wolfboot_public_key_dep = declare_dependency(
  link_with: libpublic_key,
  include_directories: inc_dirs,
  compile_args: c_args,
  dependencies: generated_inc,  # Include generated headers
)

# Complete wolfboot dependency with all components
wolfboot_complete_dep = declare_dependency(
  link_with: [libwolfboot, libwolfboothal, libpublic_key],
  include_directories: inc_dirs,
  compile_args: c_args,
  dependencies: [wolfcrypt_dep, generated_inc],  # Include generated headers
)

# Make libraries available as variables for direct access
meson.override_dependency('wolfboot', wolfboot_dep)
meson.override_dependency('wolfboot-complete', wolfboot_complete_dep)
meson.override_dependency('wolfboothal', wolfboothal_dep)
meson.override_dependency('wolfboot-public-key', wolfboot_public_key_dep)

# Also make individual libraries available
wolfboot_libraries = {
  'libwolfboot': libwolfboot,
  'libwolfboot_main': libwolfboot_main,
  'libwolfboothal': libwolfboothal,
  'libpublic_key': libpublic_key,
}

# Export important configuration variables for parent projects
wolfboot_config = {
  'arch': arch,
  'target': target,
  'sign': sign,
  'hash_alg': hash_alg,
  'c_args': c_args,
  'inc_dirs': inc_dirs,
  'image_signature_size': image_signature_size,
  'image_header_size': image_header_size,
  'target_h': target_h,  # Export the generated target.h for subprojects
}

message('wolfBoot configured successfully')
