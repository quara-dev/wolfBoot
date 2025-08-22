# Stage1 loader Meson build

fs = import('fs')

if meson.is_subproject()
  error('stage1 should be built from the top-level project')
endif

arch = get_option('arch')
target = get_option('target')

# Only build for non-library targets
if target != 'library'
  # Require a non-zero RAM load address; otherwise skip defining targets
  stg_load = get_option('wolfboot_stage1_load_addr')
  if stg_load == '0' or stg_load == '0x0'
    message('Stage1: skipped (set -Dwolfboot_stage1_load_addr to a non-zero RAM address to enable)')
    subdir_done()
  endif
  # Start from root c_args to inherit all macros (hash/sign, ORIGIN, offsets)
  c_args_stage1 = c_args + [
    '-DBUILD_LOADER_STAGE1',
    '-DWOLFBOOT_TPM_NO_CHG_PLAT_AUTH',
  ]

  # Optimize for headless embedded like Makefile
  if get_option('use_gcc_headless')
    c_args_stage1 += [
      '-Wall','-Wextra','-Werror','-Wno-main','-Wno-unused',
      '-ffunction-sections','-fdata-sections',
      '-nostdlib',
    ]
  endif

  # Stage1 sizes/addresses from options (defaults picked in root configure data)
  c_args_stage1 += [
    '-DWOLFBOOT_STAGE1_SIZE=@0@'.format(get_option('wolfboot_stage1_size')),
    '-DWOLFBOOT_STAGE1_LOAD_ADDR=@0@'.format(get_option('wolfboot_stage1_load_addr')),
    '-DWOLFBOOT_STAGE1_FLASH_ADDR=@0@'.format(get_option('wolfboot_stage1_flash_addr')),
    '-DWOLFBOOT_STAGE1_BASE_ADDR=@0@'.format(get_option('wolfboot_stage1_base_addr')),
  ]

  # Include dirs (root and wolfssl)
  incs_stage1 = [inc_dirs, build_inc_dirs]

  # Objects roughly matching Makefile when PPC or others
  stage1_sources = ['loader_stage1.c', '../src/string.c']
  if fs.exists('../hal/@0@.c'.format(target))
    stage1_sources += ['../hal/@0@.c'.format(target)]
  endif

  # Linker script: configure from hal/<target>_stage1.ld if present
  lscript_in = '../hal/@0@_stage1.ld'.format(target)
  lscript = 'loader_stage1.ld'
  if fs.exists(lscript_in)
    lconf = configuration_data()
    lconf.set('ARCH_FLASH_OFFSET', get_option('arch_flash_offset'))
    lconf.set('BOOTLOADER_PARTITION_SIZE', get_option('wolfboot_partition_size'))
    lconf.set('WOLFBOOT_ORIGIN', get_option('wolfboot_origin'))
    lconf.set('WOLFBOOT_PARTITION_BOOT_ADDRESS', get_option('wolfboot_partition_boot_address'))
    lconf.set('WOLFBOOT_PARTITION_SIZE', get_option('wolfboot_partition_size'))
    lconf.set('WOLFBOOT_PARTITION_UPDATE_ADDRESS', get_option('wolfboot_partition_update_address'))
    lconf.set('WOLFBOOT_PARTITION_SWAP_ADDRESS', get_option('wolfboot_partition_swap_address'))
    lconf.set('WOLFBOOT_STAGE1_SIZE', get_option('wolfboot_stage1_size'))
    lconf.set('WOLFBOOT_STAGE1_LOAD_ADDR', get_option('wolfboot_stage1_load_addr'))
    lconf.set('WOLFBOOT_STAGE1_FLASH_ADDR', get_option('wolfboot_stage1_flash_addr'))
    lconf.set('WOLFBOOT_STAGE1_BASE_ADDR', get_option('wolfboot_stage1_base_addr'))
    lconf.set('IMAGE_HEADER_SIZE', get_option('image_header_size'))
    configure_file(input: lscript_in, output: lscript, configuration: lconf)
  else
  error('Stage1: missing linker script @0@. Provide hal/@1@_stage1.ld as per docs/encrypted_partitions.md'.format(lscript_in, target))
  endif

  link_args_stage1 = ['-T' + lscript]
  if get_option('use_gcc_headless')
    link_args_stage1 += ['-Wl,--gc-sections', '-Wl,-Map=loader_stage1.map']
  endif

  # Build ELF
  loader_stage1_elf = executable(
    'loader_stage1',
    stage1_sources,
    include_directories: incs_stage1,
    c_args: c_args_stage1,
    link_args: link_args_stage1,
    install: false,
    name_suffix: 'elf',
    build_by_default: false,
  )

  # Produce BIN
  objcopy = find_program('objcopy', required: true)
  loader_stage1_bin = custom_target(
    'loader_stage1.bin',
    input: loader_stage1_elf,
    output: 'loader_stage1.bin',
    command: [objcopy, '--gap-fill', get_option('fill_byte'), '-O', 'binary', '@INPUT@', '@OUTPUT@'],
    build_by_default: false,
  )

  # Export convenience alias
  stage1_target = loader_stage1_bin
  set_variable('stage1_target', stage1_target)
endif
