project('wolfboot', 'c',
  version : '2.5.0',
  license : 'GPL-2.0+',
  meson_version : '>=1.3.0',
  default_options : {
    'c_std': 'c99',
    'warning_level': '2',
    'buildtype': 'release',
    'wrap_mode': 'nodownload',
    'default_library': 'static',
    'b_staticpic': false,  # Useful for embedded targets
    'b_pie': false,        # Disable PIE for embedded
    'b_lto': false,        # Disable LTO by default for embedded
    'b_ndebug': 'if-release',  # Modern Meson 1.3+ feature
  }
)

if meson.is_subproject() == false
    # Initialize global variables and modules
    cc = meson.get_compiler('c')
endif

fs = import('fs')
pkgconfig = import('pkgconfig')

# Modern Meson 1.3+ feature detection
compiler_info = {
  'id': cc.get_id(),
  'version': cc.version(),
  'has_multi_arguments': cc.has_multi_arguments(['-Wall', '-Wextra']),
  'sizeof_voidp': cc.sizeof('void*'),
}

# Enhanced feature detection and validation
host_system = host_machine.system()
host_cpu_family = host_machine.cpu_family()
target_system = target_machine.system()
target_cpu_family = target_machine.cpu_family()

# Modern environment detection
build_env = {
  'is_cross_build': meson.is_cross_build(),
  'host_endian': host_machine.endian(),
  'target_endian': target_machine.endian(),
  'build_machine_system': build_machine.system(),
}

# Project directories
project_source_root = meson.project_source_root()
project_build_root = meson.project_build_root()

# Enhanced tool detection with version checking (Meson 1.3+)
required_tools = ['objcopy', 'objdump', 'size', 'strip']
found_tools = {}

foreach tool : required_tools
  prog = find_program(tool, required: false)
  found_tools += {tool: prog}
  
  if prog.found()
    # Try to get version information where possible
    if tool in ['objcopy', 'objdump']
      try_version = run_command(prog, '--version', check: false, capture: true)
      if try_version.returncode() == 0
        version_output = try_version.stdout().strip()
        message('Found @0@: @1@'.format(tool, version_output.split('\n')[0]))
      else
        message('Found @0@ (version unknown)'.format(tool))
      endif
    else
      message('Found @0@'.format(tool))
    endif
  else
    warning('Tool @0@ not found, some features may not work'.format(tool))
  endif
endforeach

# Additional tools for specific targets
optional_tools = {
  'gdb': 'GNU Debugger for debugging',
  'openocd': 'OpenOCD for flashing and debugging',
  'st-link': 'STLink utilities for STM32',
}

foreach tool, desc : optional_tools
  prog = find_program(tool, required: false)
  if prog.found()
    message('Found optional tool @0@: @1@'.format(tool, desc))
    found_tools += {tool: prog}
  endif
endforeach

# Configuration options - using structured approach with validation
build_options = {
  'arch': get_option('arch'),
  'target': get_option('target'),
  'sign': get_option('sign'),
  'hash': get_option('hash'),
  'debug': get_option('wolfboot_debug'),
  'encrypt': get_option('encrypt'),
  'big_endian': get_option('big_endian'),
  'use_gcc_headless': get_option('use_gcc_headless'),
  'flash_otp_keystore': get_option('flash_otp_keystore'),
  'wolfboot_tpm_verify': get_option('wolfboot_tpm_verify'),
  'measured_boot': get_option('measured_boot'),
  'wolfboot_tmp_keystore': get_option('wolfboot_tmp_keystore'),
  'wolfboot_tpm_keystore': get_option('wolfboot_tpm_keystore'),
  'wolfboot_tpm_seal': get_option('wolfboot_tpm_seal'),
  'ext_flash': get_option('ext_flash'),
  'spi_flash': get_option('spi_flash'),
  'qspi_flash': get_option('qspi_flash'),
  'octospi_flash': get_option('octospi_flash'),
  'uart_flash': get_option('uart_flash'),
  'allow_downgrade': get_option('allow_downgrade'),
  'nvm_flash_writeonce': get_option('nvm_flash_writeonce'),
  'disable_backup': get_option('disable_backup'),
  'spmath': get_option('spmath'),
  'spmathall': get_option('spmathall'),
  'ram_code': get_option('ram_code'),
  'dualbank_swap': get_option('dualbank_swap'),
  'no_loader': get_option('no_loader'),
  'delta_updates': get_option('delta_updates'),
  'encrypt_with_chacha': get_option('encrypt_with_chacha'),
  'encrypt_with_aes128': get_option('encrypt_with_aes128'),
  'encrypt_with_aes256': get_option('encrypt_with_aes256'),
  'debug_uart': get_option('debug_uart'),
  'verbose': get_option('verbose'),
  'no_asm': get_option('no_asm'),
  'no_arm_asm': get_option('no_arm_asm'),
  'vtor': get_option('vtor'),
  'no_mpu': get_option('no_mpu'),
  'tzen': get_option('tzen'),
  'fsp': get_option('fsp'),
  'no_xip': get_option('no_xip'),
  'cortex_m0': get_option('cortex_m0'),
  'cortex_m3': get_option('cortex_m3'),
  'cortex_m33': get_option('cortex_m33'),
  'cortex_m7': get_option('cortex_m7'),
}

# Enhanced configuration validation with modern Meson 1.3+ features
valid_arch_targets = {
  'ARM': ['stm32f4', 'stm32f7', 'stm32h7', 'stm32l0', 'stm32l4', 'stm32wb', 'stm32u5', 
          'stm32g0', 'stm32c0', 'stm32h5', 'nrf52', 'nrf5340', 'kinetis', 'mcxa', 'mcxw',
          'same51', 'samr21', 'psoc6', 'cc26x2', 'lpc', 'imx_rt', 'rp2350'],
  'AARCH64': ['zynqmp', 'raspberrypi', 'imx8', 'raspi3', 'sama5d3'],
  'x86_64': ['x86_fsp', 'x86_64_efi', 'kontron_vx3060_s2'],
  'PPC': ['nxp_p1021', 'nxp_t1024', 'nxp_t2080', 'nxp_ls1028a'],
  'RISCV': ['hifive1', 'fe310', 'k210', 'polarfire'],
}

# Modern configuration validation
config_validator = {
  'sign_algorithms': ['NONE', 'ED25519', 'ECC256', 'ECC384', 'ECC512', 'RSA2048', 'RSA4096', 'LMS', 'XMSS', 'ML_DSA'],
  'hash_algorithms': ['SHA256', 'SHA3', 'BLAKE2B'],
  'encrypt_algorithms': ['NONE', 'CHACHA', 'AES128', 'AES256'],
}

# Validate architecture-target combination
arch = build_options['arch']
target = build_options['target']

# Enhanced validation with detailed error messages
if target != 'library'
  if arch not in valid_arch_targets
    error('Unsupported architecture: @0@. Supported: @1@'.format(arch, valid_arch_targets.keys()))
  elif target not in valid_arch_targets[arch]
    warning('Target @0@ may not be fully supported for architecture @1@. Supported targets: @2@'.format(
      target, arch, valid_arch_targets[arch]))
  else
    message('Using validated target @0@ for architecture @1@'.format(target, arch))
  endif
endif

# Validate cryptographic options
sign = build_options['sign']
hash = build_options['hash']

if sign not in config_validator['sign_algorithms']
  error('Invalid signing algorithm: @0@. Supported: @1@'.format(sign, config_validator['sign_algorithms']))
endif

if hash not in config_validator['hash_algorithms']
  error('Invalid hash algorithm: @0@. Supported: @1@'.format(hash, config_validator['hash_algorithms']))
endif

# Extract commonly used options for backward compatibility
sign = build_options['sign']
hash = build_options['hash']
debug = build_options['debug']
encrypt = build_options['encrypt']

# Configuration defines using dict approach (Meson 1.3+)
partition_config = {
  'WOLFBOOT_VERSION': get_option('wolfboot_version'),
  'WOLFBOOT_PARTITION_SIZE': get_option('wolfboot_partition_size'),
  'WOLFBOOT_SECTOR_SIZE': get_option('wolfboot_sector_size'),
  'WOLFBOOT_PARTITION_BOOT_ADDRESS': get_option('wolfboot_partition_boot_address'),
  'WOLFBOOT_PARTITION_UPDATE_ADDRESS': get_option('wolfboot_partition_update_address'),
  'WOLFBOOT_PARTITION_SWAP_ADDRESS': get_option('wolfboot_partition_swap_address'),
  'WOLFBOOT_LOAD_ADDRESS': get_option('wolfboot_load_address'),
  'WOLFBOOT_DTS_BOOT_ADDRESS': get_option('wolfboot_partition_boot_address'),
  'WOLFBOOT_DTS_UPDATE_ADDRESS': get_option('wolfboot_partition_update_address'),
  'WOLFBOOT_LOAD_DTS_ADDRESS': get_option('wolfboot_load_address'),
  'WOLFBOOT_ORIGIN': get_option('wolfboot_origin'),
  'ARCH_FLASH_OFFSET': get_option('arch_flash_offset'),
  'IMAGE_HEADER_SIZE': get_option('image_header_size'),
  'IMAGE_SIGNATURE_SIZE': get_option('image_signature_size'),
}

conf_data = configuration_data(partition_config)

# Modern compiler flag management with enhanced validation
base_flags = [
  '-D__WOLFBOOT',
  '-DWOLFSSL_USER_SETTINGS',
  '-DWOLFTPM_USER_SETTINGS',
]

# Smart warning flags based on compiler capability (Meson 1.3+)
potential_warning_flags = [
  '-Wall',
  '-Wextra', 
  '-Werror',
  '-Wno-array-bounds',
  '-Wno-main',
  '-Wno-unused',
  '-Wshadow',
  '-Wformat-security',
  '-Wstrict-prototypes',
]

# Only use warning flags that the compiler supports
warning_flags = []
foreach flag : potential_warning_flags
  if cc.has_argument(flag)
    warning_flags += [flag]
  else
    message('Compiler does not support warning flag: @0@'.format(flag))
  endif
endforeach

# Enhanced embedded flags with compiler capability detection
potential_embedded_flags = [
  '-ffreestanding',
  '-nostartfiles',
  '-ffunction-sections',
  '-fdata-sections',
  '-fomit-frame-pointer',
  '-fstack-usage',  # Modern stack analysis
  '-fno-common',
]

embedded_flags = []
if build_options['use_gcc_headless']
  foreach flag : potential_embedded_flags
    if cc.has_argument(flag)
      embedded_flags += [flag]
    endif
  endforeach
endif

# Optimization flags based on build type (Meson 1.3+ feature)
optimization_flags = []
if get_option('buildtype') == 'release'
  potential_opt_flags = ['-Os', '-fno-unwind-tables', '-fno-asynchronous-unwind-tables']
  foreach flag : potential_opt_flags
    if cc.has_argument(flag)
      optimization_flags += [flag]
    endif
  endforeach
endif

# Combine all compiler flags intelligently
c_args = base_flags + warning_flags + embedded_flags + optimization_flags

# Debug flags using modern ternary operator with enhanced options
debug_flags = build_options['debug'] ? ['-g', '-DDEBUG=1', '-DWOLFBOOT_DEBUG=1'] : []

# Additional debug flags for development builds
if build_options['debug']
  potential_debug_flags = ['-ggdb3', '-fno-omit-frame-pointer', '-fsanitize=address']
  foreach flag : potential_debug_flags
    if cc.has_argument(flag) and not meson.is_cross_build()
      debug_flags += [flag]
    endif
  endforeach
endif

c_args += debug_flags

# Verbose debug flags
c_args += build_options['debug_uart'] ? ['-DDEBUG_UART=1'] : []

# Architecture-specific optimizations
if target_cpu_family == 'arm' and cc.has_argument('-mthumb')
  c_args += ['-mthumb']
endif

# Enhanced include directories with automatic discovery
include_base_dirs = [
  '.',
  'include',
  'lib/wolfssl',
  'lib/wolfssl/wolfcrypt',
]

# Automatically discover additional include directories
additional_includes = []
if fs.exists('lib/wolftpm/wolftpm')
  additional_includes += ['lib/wolftpm']
endif

if fs.exists('lib/wolfHSM')
  additional_includes += ['lib/wolfHSM']
endif

# Target-specific includes
if target != 'library' and fs.exists('hal/@0@'.format(target))
  additional_includes += ['hal/@0@'.format(target)]
endif

inc_dirs = include_directories(include_base_dirs + additional_includes)

# Enhanced dependency management (Meson 1.3+)
project_deps = []

# Optional dependency detection
wolfssl_dep = dependency('wolfssl', required: false, method: 'pkg-config')
if wolfssl_dep.found()
  project_deps += [wolfssl_dep]
  message('Using system wolfSSL library')
else
  message('Using bundled wolfSSL sources')
endif

wolftpm_dep = dependency('wolftpm', required: false, method: 'pkg-config') 
if wolftpm_dep.found()
  project_deps += [wolftpm_dep]
  message('Using system wolfTPM library')
endif

# Math library selection using structured approach
math_config = {
  'use_spmath': build_options['spmath'],
  'use_spmath_all': build_options['spmathall'],
  'use_fastmath': not build_options['spmath'],
}

# Math sources configuration
math_sources = []
if math_config['use_spmath']
  c_args += ['-DWOLFSSL_SP_MATH']
  if math_config['use_spmath_all']
    c_args += ['-DWOLFSSL_SP_MATH_ALL']
  endif
  math_sources += ['lib/wolfssl/wolfcrypt/src/sp_int.c']
else
  c_args += ['-DUSE_FAST_MATH']
  math_sources += [
    'lib/wolfssl/wolfcrypt/src/integer.c',
    'lib/wolfssl/wolfcrypt/src/tfm.c',
  ]
endif

# Core WolfCrypt sources
wolfcrypt_sources = [
  'lib/wolfssl/wolfcrypt/src/sha256.c',
  'lib/wolfssl/wolfcrypt/src/hash.c',
  'lib/wolfssl/wolfcrypt/src/memory.c',
  'lib/wolfssl/wolfcrypt/src/wc_port.c',
  'lib/wolfssl/wolfcrypt/src/wolfmath.c',
  'lib/wolfssl/wolfcrypt/src/asn.c',
  'lib/wolfssl/wolfcrypt/src/coding.c',
] + math_sources

# Architecture-specific configuration using modern dict operations
arch_configs = {
  'ARM': {
    'flags': ['-DARCH_ARM'],
    'boot_src': 'src/boot_arm.c',
    'asm_src': 'src/boot_arm32_start.S',
  },
  'AARCH64': {
    'flags': ['-DARCH_AARCH64'],
    'boot_src': 'src/boot_aarch64.c',
    'asm_src': 'src/boot_aarch64_start.S',
  },
  'x86_64': {
    'flags': ['-DARCH_x86_64', '-DFAST_MEMCPY'],
    'boot_src': 'src/boot_x86_64.c',
    'asm_src': 'src/boot_x86_fsp_start.S',
  },
  'PPC': {
    'flags': ['-DARCH_PPC'],
    'boot_src': 'src/boot_ppc.c',
    'asm_src': 'src/boot_ppc_start.S',
  },
  'RISCV': {
    'flags': ['-DARCH_RISCV'],
    'boot_src': 'src/boot_riscv.c',
    'asm_src': 'src/vector_riscv.S',
  },
}

# Apply architecture-specific configuration
if arch in arch_configs
  arch_config = arch_configs[arch]
  c_args += arch_config['flags']
  
  # Additional ARM-specific configuration
  if arch == 'ARM'
    cortex_flags = []
    if build_options['cortex_m0']
      cortex_flags += ['-DCORTEX_M0']
    elif build_options['cortex_m3']
      cortex_flags += ['-DCORTEX_M3']
    elif build_options['cortex_m33']
      cortex_flags += ['-DCORTEX_M33']
    elif build_options['cortex_m7']
      cortex_flags += ['-DCORTEX_M7']
    endif
    
    if target.contains('cortex_m') or cortex_flags.length() > 0
      c_args += ['-DARCH_CORTEX_M'] + cortex_flags
    endif
    
    # TrustZone support
    if build_options['tzen']
      c_args += ['-DTZEN=1']
    endif
    
    # MPU support
    if not build_options['no_mpu']
      c_args += ['-DWOLFBOOT_MPU=1']
    endif
    
    # Vector table relocation
    if build_options['vtor']
      c_args += ['-DVTOR=1']
    endif
  endif
endif

# Signing algorithm configuration using modern dict operations
sign_configs = {
  'ED25519': {
    'flags': ['-DWOLFBOOT_SIGN_ED25519'],
    'sources': ['lib/wolfssl/wolfcrypt/src/ed25519.c', 'lib/wolfssl/wolfcrypt/src/ge_low_mem.c'],
  },
  'ECC256': {
    'flags': ['-DWOLFBOOT_SIGN_ECC256'],
    'sources': ['lib/wolfssl/wolfcrypt/src/ecc.c'],
  },
  'ECC384': {
    'flags': ['-DWOLFBOOT_SIGN_ECC384'],
    'sources': ['lib/wolfssl/wolfcrypt/src/ecc.c'],
  },
  'ECC512': {
    'flags': ['-DWOLFBOOT_SIGN_ECC512'],
    'sources': ['lib/wolfssl/wolfcrypt/src/ecc.c'],
  },
  'RSA2048': {
    'flags': ['-DWOLFBOOT_SIGN_RSA2048'],
    'sources': ['lib/wolfssl/wolfcrypt/src/rsa.c'],
  },
  'RSA4096': {
    'flags': ['-DWOLFBOOT_SIGN_RSA4096'],
    'sources': ['lib/wolfssl/wolfcrypt/src/rsa.c'],
  },
  'LMS': {
    'flags': ['-DWOLFBOOT_SIGN_LMS'],
    'sources': ['lib/wolfssl/wolfcrypt/src/lms.c'],
  },
  'XMSS': {
    'flags': ['-DWOLFBOOT_SIGN_XMSS'],
    'sources': ['lib/wolfssl/wolfcrypt/src/xmss.c'],
  },
  'ML_DSA': {
    'flags': ['-DWOLFBOOT_SIGN_ML_DSA', '-DML_DSA_LEVEL=@0@'.format(get_option('ml_dsa_level'))],
    'sources': ['lib/wolfssl/wolfcrypt/src/dilithium.c'],
  },
}

# Apply signing configuration
if sign != 'NONE' and sign in sign_configs
  sign_config = sign_configs[sign]
  c_args += sign_config['flags']
  wolfcrypt_sources += sign_config['sources']
endif

# Hash algorithm configuration using modern dict operations
hash_configs = {
  'SHA256': {
    'flags': ['-DWOLFBOOT_HASH_SHA256'],
    'sources': [],  # SHA256 already included in base sources
  },
  'SHA3': {
    'flags': ['-DWOLFBOOT_HASH_SHA3'],
    'sources': ['lib/wolfssl/wolfcrypt/src/sha3.c'],
  },
  'BLAKE2B': {
    'flags': ['-DWOLFBOOT_HASH_BLAKE2B'],
    'sources': ['lib/wolfssl/wolfcrypt/src/blake2b.c'],
  },
}

# Apply hash configuration
if hash in hash_configs
  hash_config = hash_configs[hash]
  c_args += hash_config['flags']
  wolfcrypt_sources += hash_config['sources']
endif

# Encryption configuration
if encrypt
  c_args += ['-DENCRYPT=1']
  
  # Encryption algorithm selection
  if build_options['encrypt_with_chacha']
    c_args += ['-DENCRYPT_WITH_CHACHA=1']
    wolfcrypt_sources += ['lib/wolfssl/wolfcrypt/src/chacha.c']
  elif build_options['encrypt_with_aes128']
    c_args += ['-DENCRYPT_WITH_AES128=1']
    wolfcrypt_sources += ['lib/wolfssl/wolfcrypt/src/aes.c']
  elif build_options['encrypt_with_aes256']
    c_args += ['-DENCRYPT_WITH_AES256=1']
    wolfcrypt_sources += ['lib/wolfssl/wolfcrypt/src/aes.c']
  endif
endif

# Feature-specific flags using modern Meson syntax (1.3+)
feature_flags = {
  'FLASH_OTP_KEYSTORE': build_options['flash_otp_keystore'],
  'WOLFBOOT_TPM_VERIFY': build_options['wolfboot_tpm_verify'],
  'WOLFBOOT_MEASURED_BOOT': build_options['measured_boot'],
  'WOLFBOOT_TPM_KEYSTORE': build_options['wolfboot_tpm_keystore'],
  'WOLFBOOT_TPM_SEAL': build_options['wolfboot_tpm_seal'],
  'EXT_FLASH': build_options['ext_flash'],
  'SPI_FLASH': build_options['spi_flash'],
  'QSPI_FLASH': build_options['qspi_flash'],
  'OCTOSPI_FLASH': build_options['octospi_flash'],
  'UART_FLASH': build_options['uart_flash'],
  'ALLOW_DOWNGRADE': build_options['allow_downgrade'],
  'NVM_FLASH_WRITEONCE': build_options['nvm_flash_writeonce'],
  'DISABLE_BACKUP': build_options['disable_backup'],
  'RAM_CODE': build_options['ram_code'],
  'DUALBANK_SWAP': build_options['dualbank_swap'],
  'BIG_ENDIAN_ORDER': build_options['big_endian'],
  'DELTA_UPDATES': build_options['delta_updates'],
  'NO_XIP': build_options['no_xip'],
  'FSP': build_options['fsp'],
}

# Add feature flags to c_args using modern iteration
foreach flag, enabled : feature_flags
  c_args += enabled ? ['-D@0@=1'.format(flag)] : []
endforeach

# Assembly optimization flags
if build_options['no_asm']
  c_args += ['-DNO_ASM']
endif

if build_options['no_arm_asm'] and arch == 'ARM'
  c_args += ['-DNO_ARM_ASM']
endif

# Core wolfboot sources
wolfboot_sources = [
  'src/string.c',
  'src/image.c',
  'src/libwolfboot.c',
  'hal/hal.c',
]

# Add loader if not disabled
if not build_options['no_loader']
  wolfboot_sources += ['src/loader.c']
endif

# Add target-specific HAL with better error handling
if target != 'library'
  hal_file = 'hal/@0@.c'.format(target)
  if fs.exists(hal_file)
    wolfboot_sources += [hal_file]
    message('Using HAL: @0@'.format(hal_file))
  else
    warning('HAL file @0@ not found, using skeleton HAL'.format(hal_file))
    wolfboot_sources += ['hal/skeleton.c']
  endif
endif

# Add keystore sources
if sign != 'NONE'
  if build_options['flash_otp_keystore']
    wolfboot_sources += ['src/flash_otp_keystore.c']
  else
    wolfboot_sources += ['src/keystore.c']
  endif
endif

# TPM-related sources
tpm_features = [
  build_options['wolfboot_tpm_verify'],
  build_options['measured_boot'],
  build_options['wolfboot_tpm_keystore'],
  build_options['wolfboot_tpm_seal']
]

if tpm_features.contains(true)
  wolfboot_sources += ['src/tpm.c']
  # Add wolfTPM library sources if available
  if fs.exists('lib/wolftpm')
    inc_dirs = include_directories(inc_dirs, 'lib/wolftpm')
    # Add relevant wolfTPM sources here
  endif
endif

# Flash driver sources
flash_drivers = {
  'ext_flash': 'src/spi_flash.c',
  'spi_flash': 'src/spi_flash.c',
  'qspi_flash': 'src/qspi_flash.c',
  'uart_flash': 'src/uart_flash.c',
}

foreach driver, source : flash_drivers
  if build_options[driver]
    wolfboot_sources += [source]
  endif
endforeach

# Platform-specific boot sources
boot_sources = []
asm_sources = []

if arch in arch_configs
  arch_config = arch_configs[arch]
  if fs.exists(arch_config['boot_src'])
    boot_sources += [arch_config['boot_src']]
  endif
  if fs.exists(arch_config['asm_src'])
    asm_sources += [arch_config['asm_src']]
  endif
endif

# Delta updates support
if build_options['delta_updates']
  if fs.exists('src/delta.c')
    wolfboot_sources += ['src/delta.c']
  endif
endif

# Generate target.h from template if it exists
target_h = []
if fs.exists('include/target.h.in')
  target_h_template = files('include/target.h.in')
  target_h = configure_file(
    input : target_h_template,
    output : 'target.h',
    configuration : conf_data,
    install : false,
  )
endif

# All sources combined
all_sources = wolfboot_sources + wolfcrypt_sources + boot_sources + asm_sources

# Main wolfboot library
libwolfboot = static_library('wolfboot',
  wolfboot_sources + wolfcrypt_sources + asm_sources,
  include_directories : inc_dirs,
  c_args : c_args,
  dependencies : [],
  install : false,
)

# Subprojects for tools (after library is defined)
subdir('tools/keytools')
subdir('tools/bin-assemble')

# Main wolfboot executable (if not library target)
if target != 'library'
  # Determine linker script
  linker_script = ''
  link_args = []
  
  # Try architecture-specific linker script
  arch_linker_script = 'hal/@0@.ld'.format(target)
  if fs.exists(arch_linker_script)
    linker_script = arch_linker_script
  elif arch == 'ARM' and fs.exists('hal/ARM.ld')
    linker_script = 'hal/ARM.ld'
  elif arch == 'RISCV' and fs.exists('hal/RISCV.ld')
    linker_script = 'hal/RISCV.ld'
  elif fs.exists('hal/@0@.ld'.format(arch))
    linker_script = 'hal/@0@.ld'.format(arch)
  endif
  
  if linker_script != ''
    link_args = ['-T', meson.current_source_dir() / linker_script]
    message('Using linker script: @0@'.format(linker_script))
  endif

  wolfboot_exe = executable('wolfboot',
    all_sources,
    include_directories : inc_dirs,
    c_args : c_args,
    link_args : link_args,
    install : false,
    name_suffix : 'elf',
  )

  # Binary generation
  wolfboot_bin = custom_target('wolfboot.bin',
    input : wolfboot_exe,
    output : 'wolfboot.bin',
    command : [find_program('objcopy'), '-O', 'binary', '@INPUT@', '@OUTPUT@'],
    install : false,
  )

  # Hex file generation
  wolfboot_hex = custom_target('wolfboot.hex',
    input : wolfboot_exe,
    output : 'wolfboot.hex',
    command : [find_program('objcopy'), '-O', 'ihex', '@INPUT@', '@OUTPUT@'],
    install : false,
  )

  # SREC file generation
  wolfboot_srec = custom_target('wolfboot.srec',
    input : wolfboot_exe,
    output : 'wolfboot.srec',
    command : [find_program('objcopy'), '-O', 'srec', '@INPUT@', '@OUTPUT@'],
    install : false,
  )
endif

# Test application
# if target != 'library'
#   subdir('test-app')
# endif

libwolfboot_dep = declare_dependency(
  include_directories: [inc_dirs],
  # compile_args: cflags,

  link_with: libwolfboot,
)

# Summary
summary({
  'Architecture' : arch,
  'Target' : target,
  'Signing' : sign,
  'Hash' : hash,
  'Debug' : debug,
  'Encryption' : encrypt,
  'TPM Support' : build_options['wolfboot_tpm_verify'] or build_options['measured_boot'],
  'Flash OTP Keystore' : build_options['flash_otp_keystore'],
}, section : 'Configuration')
