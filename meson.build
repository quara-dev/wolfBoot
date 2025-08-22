project(
  'wolfboot',
  ['c', 'cpp'],
  version: '2.5.0',
  license: 'GPL-2.0+',
  meson_version: '>=1.3.0',
  default_options: {
    'c_std': 'gnu11',
    'build.c_std': 'gnu11',
    'cpp_std': 'c++17',
    'build.cpp_std': 'c++17',
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
build_keytools = get_option('build_keytools')
build_bin_assemble = get_option('build_bin_assemble')
external_public_key = get_option('external_public_key')

# Helper function to get keygen flag from signing algorithm
keygen_flags = {
  'ECC256': '--ecc256',
  'ECC384': '--ecc384',
  'ECC521': '--ecc521',
  'ED25519': '--ed25519',
  'ED448': '--ed448',
  'RSA2048': '--rsa2048',
  'RSA3072': '--rsa3072',
  'RSA4096': '--rsa4096',
}

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
  '-Wno-unused-parameter',
  '-Wno-pointer-to-int-cast',
  '-Wno-int-to-pointer-cast',
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
  # Apply Cortex-M tuning like arch.mk
  if cc.has_argument('-mthumb')
    c_args += ['-mthumb']
  endif
  if cc.has_argument('-mlittle-endian')
    c_args += ['-mlittle-endian']
  endif
  if cc.has_argument('-mthumb-interwork')
    c_args += ['-mthumb-interwork']
  endif
  # CPU
  cpu_flag = ''
  if get_option('cortex_m33')
    cpu_flag = '-mcpu=cortex-m33'
  elif get_option('cortex_m7')
    cpu_flag = '-mcpu=cortex-m7'
  elif get_option('cortex_m3')
    cpu_flag = '-mcpu=cortex-m3'
  elif get_option('cortex_m0')
    cpu_flag = '-mcpu=cortex-m0'
  else
    cpu_flag = '-mcpu=cortex-m4'
  endif
  if cc.has_argument(cpu_flag)
    c_args += [cpu_flag]
  endif
  # Add ARM-specific embedded flags (matching CMakeLists.txt)
  if get_option('use_gcc_headless')
    c_args += ['-ffreestanding', '-nostartfiles', '-fomit-frame-pointer']
  endif
elif arch == 'AARCH64'
  c_args += ['-DARCH_AARCH64']
  # Add additional AARCH64 flags from CMakeLists.txt
  c_args += ['-DNO_QNX', '-DWOLFBOOT_DUALBOOT', '-DMMU']
elif arch == 'x86_64'
  c_args += ['-DARCH_x86_64', '-DFAST_MEMCPY']
  # Add x86_64 EFI target flag
  if target == 'x86_64_efi'
    c_args += ['-DTARGET_X86_64_EFI']
    if debug
      c_args += ['-DWOLFBOOT_DEBUG_EFI=1']
    endif
  endif
elif arch == 'PPC'
  c_args += ['-DARCH_PPC']
elif arch == 'RISCV'
  c_args += ['-DARCH_RISCV']
endif

# Add target-specific definition (matching CMakeLists.txt)
c_args += ['-DTARGET_@0@'.format(target.to_upper())]

# Add ARCH_FLASH_OFFSET (matching CMakeLists.txt)
arch_flash_offset = get_option('arch_flash_offset')
c_args += ['-DARCH_FLASH_OFFSET=@0@'.format(arch_flash_offset)]

# Add WOLFBOOT_ORIGIN handling
wolfboot_origin = get_option('wolfboot_origin')
if wolfboot_origin == ''
  c_args += ['-DWOLFBOOT_ORIGIN=@0@'.format(arch_flash_offset)]
else
  c_args += ['-DWOLFBOOT_ORIGIN=@0@'.format(wolfboot_origin)]
endif

# Signing algorithm flags with stack usage and image header size (matching CMakeLists.txt)
image_header_size = get_option('image_header_size')
stack_usage = 1216 # Default value

if sign == 'ECC256'
  c_args += ['-DWOLFBOOT_SIGN_ECC256']
  if get_option('wolfboot_small_stack')
    stack_usage = 4096
  elif not get_option('spmath')
    stack_usage = 5264
  else
    stack_usage = 7632
  endif
  if image_header_size < 256
    image_header_size = 256
  endif
elif sign == 'ECC384'
  c_args += ['-DWOLFBOOT_SIGN_ECC384']
  if get_option('wolfboot_small_stack')
    stack_usage = 5880
  elif not get_option('spmath')
    stack_usage = 11248
  else
    stack_usage = 11216
  endif
  if image_header_size < 512
    image_header_size = 512
  endif
elif sign == 'ECC521'
  c_args += ['-DWOLFBOOT_SIGN_ECC521']
  if get_option('wolfboot_small_stack')
    stack_usage = 4096
  elif not get_option('spmath')
    stack_usage = 7352
  else
    stack_usage = 3896
  endif
  if image_header_size < 512
    image_header_size = 512
  endif
elif sign == 'ED25519'
  c_args += ['-DWOLFBOOT_SIGN_ED25519']
  stack_usage = 5000
  if image_header_size < 256
    image_header_size = 256
  endif
elif sign == 'ED448'
  c_args += ['-DWOLFBOOT_SIGN_ED448']
  if get_option('wolfboot_small_stack')
    stack_usage = 1024
  else
    stack_usage = 4376
  endif
  if image_header_size < 512
    image_header_size = 512
  endif
elif sign == 'RSA2048'
  c_args += ['-DWOLFBOOT_SIGN_RSA2048']
  if get_option('wolfboot_small_stack')
    if not get_option('spmath')
      stack_usage = 5008
    else
      stack_usage = 4096
    endif
  elif not get_option('spmath')
    stack_usage = 35952
  else
    stack_usage = 17568
  endif
  if image_header_size < 512
    image_header_size = 512
  endif
elif sign == 'RSA3072'
  c_args += ['-DWOLFBOOT_SIGN_RSA3072']
elif sign == 'RSA4096'
  c_args += ['-DWOLFBOOT_SIGN_RSA4096']
  if get_option('wolfboot_small_stack')
    if not get_option('spmath')
      stack_usage = 5888
    else
      stack_usage = 5768
    endif
  elif not get_option('spmath')
    stack_usage = 69232
  else
    stack_usage = 18064
  endif
  if image_header_size < 1024
    image_header_size = 1024
  endif
elif sign == 'LMS'
  c_args += ['-DWOLFBOOT_SIGN_LMS']
  # LMS configuration flags matching options.mk
  lms_levels = get_option('lms_levels')
  lms_height = get_option('lms_height')
  lms_winternitz = get_option('lms_winternitz')

  c_args += [
    '-DWOLFSSL_HAVE_LMS',
    '-DWOLFSSL_WC_LMS',
    '-DWOLFSSL_WC_LMS_SMALL',
    '-DWOLFSSL_LMS_MAX_LEVELS=@0@'.format(lms_levels),
    '-DWOLFSSL_LMS_MAX_HEIGHT=@0@'.format(lms_height),
    '-DLMS_LEVELS=@0@'.format(lms_levels),
    '-DLMS_HEIGHT=@0@'.format(lms_height),
    '-DLMS_WINTERNITZ=@0@'.format(lms_winternitz),
    '-DLMS_IMAGE_SIGNATURE_SIZE=@0@'.format(get_option('image_signature_size')),
    '-DWOLFSSL_LMS_VERIFY_ONLY',
  ]
elif sign == 'XMSS'
  c_args += ['-DWOLFBOOT_SIGN_XMSS']
elif sign == 'ML_DSA'
  c_args += ['-DWOLFBOOT_SIGN_ML_DSA']
elif sign == 'NONE'
  c_args += ['-DWOLFBOOT_NO_SIGN']
  if hash == 'SHA384'
    stack_usage = 3760
  else
    stack_usage = 1216
  endif
endif

# Add IMAGE_HEADER_SIZE and stack usage warnings (matching CMakeLists.txt)
c_args += ['-DIMAGE_HEADER_SIZE=@0@'.format(image_header_size)]

# Also export BOOTLOADER_PARTITION_SIZE like Makefile
c_args += ['-DBOOTLOADER_PARTITION_SIZE=@0@'.format(get_option('wolfboot_partition_size'))]

# Add stack usage warning if supported by compiler
if cc.has_argument('-Wstack-usage=@0@'.format(stack_usage))
  c_args += ['-Wstack-usage=@0@'.format(stack_usage)]
endif

# Add other warnings from CMakeLists.txt
if cc.has_argument('-Wno-unused')
  c_args += ['-Wno-unused']
endif

# Hash algorithm flags
if hash == 'SHA256'
  c_args += ['-DWOLFBOOT_HASH_SHA256']
elif hash == 'SHA384'
  c_args += ['-DWOLFBOOT_HASH_SHA384']
elif hash == 'SHA3'
  c_args += ['-DWOLFBOOT_HASH_SHA3']
endif

# Encryption flags (matching CMakeLists.txt)
if encrypt
  c_args += ['-DEXT_ENCRYPTED=1']
  if get_option('encrypt_with_aes128')
    c_args += ['-DENCRYPT_WITH_AES128', '-DWOLFSSL_AES_128']
  elif get_option('encrypt_with_aes256')
    c_args += ['-DENCRYPT_WITH_AES256', '-DWOLFSSL_AES_256']
  else
    # Default to ChaCha if no specific AES mode is selected (like CMakeLists.txt)
    c_args += ['-DENCRYPT_WITH_CHACHA', '-DHAVE_CHACHA']
  endif

  # Encryption cache support
  encrypt_cache = get_option('encrypt_cache')
  if encrypt_cache > 0
    c_args += ['-DWOLFBOOT_ENCRYPT_CACHE=@0@'.format(encrypt_cache)]
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
  # PCR index
  c_args += ['-DWOLFBOOT_MEASURED_PCR_A=@0@'.format(get_option('measured_pcr_a'))]
endif

if get_option('wolfboot_tpm_keystore')
  c_args += ['-DWOLFBOOT_TPM_KEYSTORE']
  if get_option('wolfboot_tpm_keystore_auth') != ''
    c_args += ['-DWOLFBOOT_TPM_KEYSTORE_AUTH=\"@0@\"'.format(get_option('wolfboot_tpm_keystore_auth'))]
  endif
  if get_option('wolfboot_tpm_keystore_nv_base') != ''
    c_args += ['-DWOLFBOOT_TPM_KEYSTORE_NV_BASE=@0@'.format(get_option('wolfboot_tpm_keystore_nv_base'))]
  endif
endif

if get_option('wolfboot_tpm_seal')
  c_args += ['-DWOLFBOOT_TPM_SEAL']
  if get_option('wolfboot_tpm_seal_auth') != ''
    c_args += ['-DWOLFBOOT_TPM_SEAL_AUTH=\"@0@\"'.format(get_option('wolfboot_tpm_seal_auth'))]
  endif
  if get_option('wolfboot_tpm_seal_nv_base') != ''
    c_args += ['-DWOLFBOOT_TPM_SEAL_NV_BASE=@0@'.format(get_option('wolfboot_tpm_seal_nv_base'))]
  endif
  if get_option('wolfboot_tpm_seal_key_id') != ''
    c_args += ['-DWOLFBOOT_TPM_SEAL_KEY_ID=@0@'.format(get_option('wolfboot_tpm_seal_key_id'))]
  endif
endif

if get_option('ext_flash')
  c_args += ['-DEXT_FLASH=1', '-DPART_UPDATE_EXT=1', '-DPART_SWAP_EXT=1']
  if get_option('no_xip')
    c_args += ['-DPART_BOOT_EXT=1']
  endif
endif

if get_option('spi_flash')
  c_args += ['-DSPI_FLASH=1']
endif

if get_option('qspi_flash')
  c_args += ['-DQSPI_FLASH=1']
endif

if get_option('octospi_flash')
  c_args += ['-DOCTOSPI_FLASH=1', '-DQSPI_FLASH=1']
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

if get_option('dualbank_swap')
  c_args += ['-DWOLFBOOT_DUALBOOT', '-DDUALBANK_SWAP=1']
endif

if get_option('debug_uart')
  c_args += ['-DDEBUG_UART']
endif

if get_option('big_endian')
  c_args += ['-DBIG_ENDIAN_ORDER']
endif

if get_option('wolfboot_small_stack')
  c_args += ['-DWOLFBOOT_SMALL_STACK', '-DXMALLOC_USER']
endif

# UART instance macros
if get_option('use_uart1')
  c_args += ['-DUSE_UART1=1']
endif
if get_option('use_uart3')
  c_args += ['-DUSE_UART3=1']
endif

# VTOR control (map vtor=false to NO_VTOR)
if not get_option('vtor')
  c_args += ['-DNO_VTOR']
endif

# Partitions off
if get_option('wolfboot_no_partitions')
  c_args += ['-DWOLFBOOT_NO_PARTITIONS']
endif

# Disk lock support
if get_option('disk_lock')
  c_args += ['-DWOLFBOOT_ATA_DISK_LOCK']
  dl_pass = get_option('disk_lock_password')
  if dl_pass != ''
    c_args += ['-DWOLFBOOT_ATA_DISK_LOCK_PASSWORD=\"@0@\"'.format(dl_pass)]
  endif
endif

# Flash multi-sector erase
if get_option('flash_multi_sector_erase')
  c_args += ['-DWOLFBOOT_FLASH_MULTI_SECTOR_ERASE']
endif

# KeyVault sizing
if get_option('keyvault_obj_size') > 0
  c_args += ['-DKEYVAULT_OBJ_SIZE=@0@'.format(get_option('keyvault_obj_size'))]
endif
if get_option('keyvault_max_items') > 0
  c_args += ['-DKEYVAULT_MAX_ITEMS=@0@'.format(get_option('keyvault_max_items'))]
endif

# Custom partition ID
if get_option('wolfboot_part_id') > 0
  c_args += ['-DHDR_IMG_TYPE_APP=@0@'.format(get_option('wolfboot_part_id'))]
endif

# Additional options from CMakeLists.txt
if get_option('flags_home')
  c_args += ['-DFLAGS_HOME=1']
endif

if get_option('flags_invert')
  c_args += ['-DWOLFBOOT_FLAGS_INVERT=1']
endif

if get_option('no_mpu')
  c_args += ['-DWOLFBOOT_NO_MPU']
endif

if get_option('no_xip')
  c_args += ['-DWOLFBOOT_NO_XIP', '-DNO_XIP']
endif

if get_option('armored')
  c_args += ['-DWOLFBOOT_ARMORED']
endif

# Misc option-to-macro mappings
if get_option('no_qnx')
  c_args += ['-DNO_QNX']
endif
if get_option('wolfboot_universal_keystore')
  c_args += ['-DWOLFBOOT_UNIVERSAL_KEYSTORE']
endif
if get_option('linux_payload')
  c_args += ['-DWOLFBOOT_LINUX_PAYLOAD']
endif
if get_option('multiboot2')
  c_args += ['-DWOLFBOOT_MULTIBOOT2']
endif
if get_option('wolfboot_64bit')
  c_args += ['-DWOLFBOOT_64BIT']
endif
if get_option('debug_tpm')
  c_args += ['-DWOLFBOOT_DEBUG_TPM=1']
endif

# ELF loader support and options
if get_option('elf')
  c_args += ['-DWOLFBOOT_ELF']
  if get_option('elf_flash_scatter')
    c_args += ['-DWOLFBOOT_ELF_FLASH_SCATTER=1']
  endif
  if get_option('debug_elf') > 0
    c_args += ['-DDEBUG_ELF=@0@'.format(get_option('debug_elf'))]
  endif
endif

# Add WOLFBOOT_VERSION (matching CMakeLists.txt)
wolfboot_version = get_option('wolfboot_version')
c_args += ['-DWOLFBOOT_VERSION=@0@'.format(wolfboot_version)]

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
  if get_option('spmathall')
    c_args += ['-DWOLFSSL_SP_MATH_ALL']
  endif
  # Note: WOLFSSL_SP_MATH is defined in user_settings.h, don't redefine here
else
  c_args += ['-DUSE_FAST_MATH']
endif

# Include directories
local_inc_dirs = include_directories('.', 'include')
wolfssl_inc_dirs = include_directories('lib/wolfssl')

inc_dirs = [
  local_inc_dirs,
  wolfssl_inc_dirs,
]

# Additional build directory for generated files
build_inc_dirs = include_directories(['.'], is_system: false)

# Update sources (matching CMakeLists.txt logic)
update_sources = []
if target == 'x86_64_efi'
  # x86_64_efi uses update_ram.c
  update_sources += ['src/update_ram.c']
else
  # Default for most targets is update_flash.c
  if get_option('dualbank_swap')
    update_sources += ['src/update_flash_hwswap.c']
  else
    update_sources += ['src/update_flash.c']
  endif
endif

# Initialize keystore sources
public_key_sources = []

# Add keystore sources based on signing configuration
if sign != 'NONE'
  if get_option('flash_otp_keystore')
    public_key_sources += ['src/flash_otp_keystore.c']
  else
    # Check if external public key is provided and keytools are built
    if external_public_key != '' and build_keytools
      external_key = meson.current_source_dir() + '/' + external_public_key
      # Ensure the external public key file exists
      if not fs.exists(external_key)
        error('External public key file not found: @0@'.format(external_key))
      endif
      
      # Get keygen flag for the signing algorithm
      keygen_sign_flag = keygen_flags.get(sign, '--ecc256')
      
      # Wait for keytools to be built first by making this depend on the keytools
      # We need to defer this until after keytools are defined
      external_keystore_configured = true
    else
      external_keystore_configured = false
      
      # Create a dummy keystore.c if it doesn't exist (will be replaced by build tools)
      if not fs.exists('src/keystore.c')
        # Create a placeholder keystore file
        keystore_placeholder = custom_target(
          'keystore_placeholder',
          output: 'keystore_placeholder.c',
          command: [
            find_program('sh'),
            '-c', 'echo "// Placeholder keystore" > @OUTPUT@',
          ],
        )
        public_key_sources += [keystore_placeholder]
      else
        public_key_sources += ['src/keystore.c']
      endif
    endif
  endif
else
  external_keystore_configured = false
  # For SIGN=NONE, create an empty keystore
  keystore_none = custom_target(
    'keystore_none',
    output: 'keystore_none.c',
    command: [find_program('sh'), '-c', 'echo "// SIGN=NONE" > @OUTPUT@'],
  )
  public_key_sources += [keystore_none]
endif

# Configure subdir to handle lib libraries
subdir('lib')

# Flash driver configuration (matching CMakeLists.txt logic)
flash_driver_sources = []

# Set EXT_FLASH based on flash drivers (like CMakeLists.txt)
ext_flash_enabled = (
  get_option('spi_flash')
  or get_option('qspi_flash')
  or get_option('octospi_flash')
  or get_option('uart_flash')
  or get_option('ext_flash')
)

if ext_flash_enabled
  c_args += ['-DEXT_FLASH=1', '-DPART_UPDATE_EXT=1', '-DPART_SWAP_EXT=1']
  if get_option('no_xip')
    c_args += ['-DPART_BOOT_EXT=1']
  endif
endif

# Flash driver sources
if get_option('spi_flash')
  c_args += ['-DSPI_FLASH=1']
  flash_driver_sources += ['src/spi_flash.c']
endif

if get_option('qspi_flash')
  c_args += ['-DQSPI_FLASH=1']
  flash_driver_sources += ['src/qspi_flash.c']
endif

if get_option('octospi_flash')
  c_args += ['-DOCTOSPI_FLASH=1', '-DQSPI_FLASH=1']
endif

if get_option('uart_flash')
  c_args += ['-DUART_FLASH=1']
  flash_driver_sources += ['src/uart_flash.c']
endif

# TPM sources
tpm_enabled = (
  get_option('wolfboot_tpm_verify')
  or get_option('measured_boot')
  or get_option('wolfboot_tpm_keystore')
  or get_option('wolfboot_tpm_seal')
)

if tpm_enabled
  c_args += ['-DWOLFBOOT_TPM', '-DWOLFTPM_SMALL_STACK', '-DWOLFTPM_AUTODETECT']

  # Add additional wolfSSL crypto sources needed for TPM to wolfcrypt
  if fs.exists('lib/wolfTPM')
    inc_dirs += include_directories('lib/wolfTPM')
    wolfcrypt_sources += [
      'lib/wolfssl/wolfcrypt/src/aes.c',
      'lib/wolfssl/wolfcrypt/src/hmac.c',
      'lib/wolfssl/wolfcrypt/src/random.c',
    ]
  endif
endif

# Delta update sources - always define for tools, conditionally include in main build
wolfboot_delta_sources = ['../../src/delta.c'] # Path relative to tools directory
if get_option('delta_updates')
  c_args += ['-DDELTA_UPDATES']

  # Add delta block size if defined
  delta_block_size = get_option('delta_block_size')
  if delta_block_size > 0
    c_args += ['-DDELTA_BLOCK_SIZE=@0@'.format(delta_block_size)]
  endif
endif

# Subprojects - Build tools separately like Makefile
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
  # Get the bin-assemble tool from the subdirectory (wrap as array)
  bin_assemble_target = [get_variable('bin_assemble_executable')]
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

# Generate external keystore if external public key is provided
if external_keystore_configured and keytools_target.length() > 0
  # Get the keygen executable
  keygen_exe = keytools_target[0]  # keygen is first in the array
  
  # Get keygen flag for the signing algorithm
  keygen_sign_flag = keygen_flags.get(sign, '--ecc256')
  
  external_key = meson.current_source_dir() + '/' + external_public_key
  # Generate keystore.c from external public key
  external_keystore = custom_target(
    'external_keystore',
    input: [external_key],
    output: ['keystore.c'],
    command: [
      keygen_exe,
      '--force',
      keygen_sign_flag,
      '-i', '@INPUT0@',
      '-keystoreDir', meson.current_build_dir(),
    ],
    depends: [keygen_exe],
    build_by_default: true,
    console: true,
  )
  
  # Replace the placeholder keystore with the generated one
  public_key_sources = [external_keystore]
  
  message('Using external public key: @0@'.format(external_public_key))
  message('Generating keystore.c with @0@ algorithm'.format(sign))
endif

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

# Build separate static libraries like CMakeLists.txt

# 2. WolfTPM library (if TPM support is enabled and not a subproject)
libwolftpm = []
wolftpm_sources = []
if tpm_enabled and fs.exists('lib/wolfTPM') and not meson.is_subproject()
  wolftpm_sources = [
    'lib/wolfTPM/src/tpm2.c',
    'lib/wolfTPM/src/tpm2_packet.c',
    'lib/wolfTPM/src/tpm2_tis.c',
    'lib/wolfTPM/src/tpm2_wrap.c',
    'lib/wolfTPM/src/tpm2_param_enc.c',
  ]

  libwolftpm = static_library(
    'wolftpm',
    wolftpm_sources,
    include_directories: [inc_dirs, build_inc_dirs],
    c_args: c_args,
    install: false,
  )
endif

# 3. WolfBoot HAL library (hardware abstraction layer) - only build if not a subproject
libwolfboothal = []
if not meson.is_subproject()
  hal_sources = ['hal/hal.c'] + flash_driver_sources
  if target != 'library'
    if fs.exists('hal/@0@.c'.format(target))
      hal_sources += ['hal/@0@.c'.format(target)]
    else
      hal_sources += ['hal/skeleton.c']
    endif
  endif

  # Add bus drivers used by flash/UART
  # SPI driver if SPI/QSPI is enabled
  if (get_option('spi_flash') or get_option('qspi_flash'))
    spi_drv = 'hal/spi/spi_drv_@0@.c'.format(get_option('spi_target'))
    if fs.exists(spi_drv)
      hal_sources += [spi_drv]
    endif
  endif
  # UART driver if UART flash is enabled
  if get_option('uart_flash')
    uart_drv = 'hal/uart/uart_drv_@0@.c'.format(get_option('uart_target'))
    if fs.exists(uart_drv)
      hal_sources += [uart_drv]
    endif
  endif

  libwolfboothal = static_library(
    'wolfboothal',
    hal_sources,
    include_directories: [inc_dirs, build_inc_dirs],
    c_args: c_args,
    install: false,
  )
endif

# 4. Public key library (keystore sources, only if signing is enabled)
libpublic_key = []
if sign != 'NONE'
  libpublic_key = static_library(
    'public_key',
    public_key_sources,
    include_directories: [inc_dirs, build_inc_dirs],
    c_args: c_args,
    install: false,
  )
endif

# 5. Main wolfboot library (core sources that link everything together)
wolfboot_sources = [
  'src/string.c',
  'src/image.c',
  'src/libwolfboot.c',
] + update_sources

# Add additional sources based on configuration
if get_option('wolfboot_small_stack')
  wolfboot_sources += ['src/xmalloc.c']
endif

if not get_option('no_loader')
  wolfboot_sources += ['src/loader.c']
endif

# Add architecture-specific boot sources
if arch == 'ARM' and target != 'library'
  wolfboot_sources += ['src/boot_arm.c']
elif arch == 'AARCH64' and target != 'library'
  wolfboot_sources += ['src/boot_aarch64.c', 'src/boot_aarch64_start.S']
elif arch == 'x86_64' and target == 'x86_64_efi'
  wolfboot_sources += ['src/boot_x86_64.c']
endif

# Add TPM sources if enabled
if tpm_enabled
  wolfboot_sources += ['src/tpm.c']
endif

# Add delta update sources if enabled
if get_option('delta_updates')
  wolfboot_sources += ['src/delta.c']
endif

# Link libraries list
link_libraries = [libwolfcrypt]
if sign != 'NONE'
  link_libraries += [libpublic_key]
endif
if not meson.is_subproject()
  link_libraries += [libwolfboothal]
  if tpm_enabled and fs.exists('lib/wolfTPM')
    link_libraries += [libwolftpm]
  endif
endif

# c_args += ['--specs=nano.specs', '--specs=nosys.specs']

libwolfboot = static_library(
  'wolfboot',
  wolfboot_sources,
  include_directories: [inc_dirs, build_inc_dirs],
  c_args: c_args + ['-Wno-unused'],
  link_with: link_libraries,
  install: false,
)

 

# Create dependency for other projects (needed by subprojects)
if meson.is_subproject()
  # For subprojects, expose individual libraries as variables
  subproject_link_libraries = [libwolfcrypt, libwolfboot]
  if sign != 'NONE'
    subproject_link_libraries += [libpublic_key]
  endif
  
  # Make individual libraries available to parent projects
  wolfboot_libwolfcrypt = libwolfcrypt
  wolfboot_libwolfboot = libwolfboot
  wolfboot_libwolfboothal = libwolfboothal
  wolfboot_libpublic_key = libpublic_key
  wolfboot_libwolftpm = libwolftpm
  
  # Make keytools executables available to parent projects
  if build_keytools and keytools_target.length() > 0
    wolfboot_keygen = keytools_target[0]  # keygen executable
    wolfboot_sign_tool = keytools_target[1]  # sign executable
    wolfboot_keytools = keytools_target  # both tools as array
  endif
  
  # Main dependency with core libraries
  wolfboot_dep = declare_dependency(
    include_directories: inc_dirs,
    link_with: subproject_link_libraries,
    compile_args: c_args,
  )
  
  # Additional dependencies for individual libraries
  wolfboot_wolfcrypt_dep = declare_dependency(
    include_directories: inc_dirs,
    link_with: [libwolfcrypt],
    compile_args: c_args,
  )
  
  wolfboot_hal_dep = declare_dependency(
    include_directories: inc_dirs,
    link_with: [libwolfboothal],
    compile_args: c_args,
  )
  
  if sign != 'NONE'
    wolfboot_pubkey_dep = declare_dependency(
      include_directories: inc_dirs,
      link_with: [libpublic_key],
      compile_args: c_args,
    )
  endif
  
  if tpm_enabled and fs.exists('lib/wolfTPM')
    wolfboot_tpm_dep = declare_dependency(
      include_directories: inc_dirs,
      link_with: [libwolftpm],
      compile_args: c_args,
    )
  endif
else
  # For standalone builds, expose all libraries
  wolfboot_dep = declare_dependency(
    include_directories: inc_dirs,
    link_with: [libwolfboot] + link_libraries,
    compile_args: c_args,
  )
endif

# Main executable (if not library target and not a subproject)
if target != 'library' and not meson.is_subproject()
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
  linker_conf_data.set('ARCH_FLASH_OFFSET', get_option('arch_flash_offset'))
  linker_conf_data.set('WOLFBOOT_ORIGIN', get_option('wolfboot_origin'))
  linker_conf_data.set('WOLFBOOT_PARTITION_SIZE', get_option('wolfboot_partition_size'))
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
  linker_conf_data.set('IMAGE_HEADER_SIZE', image_header_size)
  # Placeholders for stage1 and other advanced features
  linker_conf_data.set('WOLFBOOT_STAGE1_SIZE', '0')
  linker_conf_data.set('WOLFBOOT_STAGE1_LOAD_ADDR', '0')
  linker_conf_data.set('WOLFBOOT_STAGE1_FLASH_ADDR', '0')
  linker_conf_data.set('WOLFBOOT_STAGE1_BASE_ADDR', '0')
  linker_conf_data.set('WOLFBOOT_LOAD_BASE', '0')
  linker_conf_data.set('BOOTLOADER_START', '0')
  linker_conf_data.set('FSP_S_LOAD_BASE', '0')

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

    # Add ARM-specific linker flags (matching CMakeLists.txt)
    if arch == 'ARM'
      if cc.has_link_argument('-fomit-frame-pointer')
        link_args += ['-fomit-frame-pointer']
      endif
      if cc.has_link_argument('-mthumb')
        link_args += ['-mthumb']
      endif
      lcpu = ''
      if get_option('cortex_m33')
        lcpu = '-mcpu=cortex-m33'
      elif get_option('cortex_m7')
        lcpu = '-mcpu=cortex-m7'
      elif get_option('cortex_m3')
        lcpu = '-mcpu=cortex-m3'
      elif get_option('cortex_m0')
        lcpu = '-mcpu=cortex-m0'
      else
        lcpu = '-mcpu=cortex-m4'
      endif
      if cc.has_link_argument(lcpu)
        link_args += [lcpu]
      endif
      # Add embedded-optimized specs for smaller binary size
      # if meson.is_cross_build()
      #   link_args += ['--specs=nano.specs', '--specs=nosys.specs']
      # endif
    endif
  endif

  wolfboot_exe = executable(
    'wolfboot',
    # Only include the main source files, libraries are linked separately
    wolfboot_sources,
    include_directories: [inc_dirs, build_inc_dirs],
    c_args: c_args,
    link_args: link_args,
    link_with: link_libraries,
    install: false,
    name_suffix: 'elf',
    build_by_default: meson.is_subproject() == false,
  )

  # Post-build steps (objcopy, etc.)
  objcopy = find_program('objcopy', required: true)

  # Create wolfboot.bin from wolfboot.elf
  wolfboot_bin_tgt = custom_target(
    'wolfboot.bin',
    input: wolfboot_exe,
    output: 'wolfboot.bin',
  command: [objcopy, '--gap-fill', get_option('fill_byte'), '-O', 'binary', '@INPUT@', '@OUTPUT@'],
    build_by_default: true
  )

  # Create wolfboot.hex from wolfboot.elf
  custom_target(
    'wolfboot.hex',
    input: wolfboot_exe,
    output: 'wolfboot.hex',
    command: [objcopy, '-O', 'ihex', '@INPUT@', '@OUTPUT@'],
    build_by_default: false, # Match Makefile 'hex' target
    console: true
  )

  # Create wolfboot.srec from wolfboot.elf
  custom_target(
    'wolfboot.srec',
    input: wolfboot_exe,
    output: 'wolfboot.srec',
    command: [objcopy, '-O', 'srec', '@INPUT@', '@OUTPUT@'],
    build_by_default: false, # Match Makefile 'srec' target
    console: true
  )

  # Handle x86_64_efi target specifically
  if target == 'x86_64_efi'
    custom_target(
      'wolfboot.efi',
      input: wolfboot_exe,
      output: 'wolfboot.efi',
      command: [
        objcopy,
        '-j', '.rodata', '-j', '.text', '-j', '.sdata', '-j', '.data',
        '-j', '.dynamic', '-j', '.dynsym', '-j', '.rel',
        '-j', '.rela', '-j', '.reloc', '-j', '.eh_frame',
        '--target=efi-app-x86_64', '--subsystem=10',
        '@INPUT@', '@OUTPUT@'
      ],
      build_by_default: true
    )
  endif

endif

# Summary
libraries_built = 'wolfcrypt'
if not meson.is_subproject()
  if tpm_enabled and fs.exists('lib/wolfTPM')
    libraries_built += ', wolftpm'
  endif
  libraries_built += ', wolfboothal'
  if sign != 'NONE'
    libraries_built += ', public_key'
  endif
endif
libraries_built += ', wolfboot'

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
    'External Public Key': get_option('external_public_key') != '' ? get_option('external_public_key') : 'None',
    'Is Subproject': meson.is_subproject(),
    'Libraries': libraries_built,
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

# Documentation for subproject usage
if meson.is_subproject()
  message(
    '''
SUBPROJECT VARIABLES AVAILABLE:

Individual Libraries:
- wolfboot_libwolfcrypt     : WolfSSL cryptographic library
- wolfboot_libwolfboot      : Main wolfBoot library  
- wolfboot_libwolfboothal   : Hardware abstraction layer
- wolfboot_libpublic_key    : Public key handling (if signing enabled)
- wolfboot_libwolftpm       : TPM support (if TPM enabled)

Keytools Executables (if build_keytools=true):
- wolfboot_keygen           : Key generation executable
- wolfboot_sign_tool        : Firmware signing executable
- wolfboot_keytools         : Both tools as array [keygen, sign_tool]

Dependencies:
- wolfboot_dep              : Main dependency (wolfcrypt + wolfboot)
- wolfboot_wolfcrypt_dep    : WolfSSL crypto only
- wolfboot_hal_dep          : HAL only
- wolfboot_pubkey_dep       : Public key only (if available)
- wolfboot_tpm_dep          : TPM only (if available)

Usage in parent project:
  wolfboot_proj = subproject('wolfboot', default_options: ['build_keytools=true'])
  wolfboot_dep = wolfboot_proj.get_variable('wolfboot_dep')
  keygen_tool = wolfboot_proj.get_variable('wolfboot_keygen')
  sign_tool = wolfboot_proj.get_variable('wolfboot_sign_tool')
  ''',
  )
endif

# External public key usage information
if external_public_key != '' and not build_keytools
  warning(
    '''
WARNING: External public key specified but keytools are not built.
To use external public key, enable keytools build:
  meson configure -Dbuild_keytools=true -Dexternal_public_key=@0@

Or build directly with:
  meson setup builddir -Dbuild_keytools=true -Dexternal_public_key=@0@
  '''.format(external_public_key)
  )
elif external_public_key != '' and build_keytools
  message(
    '''
External public key configured successfully!
- Key file: @0@
- Signing algorithm: @1@
- Generated keystore.c will be used for signing verification

The keystore.c file will be automatically generated from your public key.
    '''.format(external_public_key, sign)
  )
endif

if target == 'rp2350'
  message('To complete the build, check IDE/pico-sdk/rp2350')
endif

# Include test-app only in non-library mode
if target != 'library' and not meson.is_subproject()
  subdir('test-app')
  subdir('stage1')
endif


