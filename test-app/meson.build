# Minimal Meson build for test-app to provide image.elf/bin for packaging

arch = get_option('arch')
target = get_option('target')

if meson.is_subproject()
	error('test-app should be built from the top-level project')
endif

# Only build a simple simulated/standalone app when target is library or arch is sim
app_sources = []

if arch == 'ARM' or arch == 'AARCH64' or arch == 'RISCV' or arch == 'PPC' or arch == 'x86_64'
	# Include basic platform-agnostic demo sources (timer/led/system)
	common = [
		'system.c',
		'timer.c',
		'led.c',
	]
	# Pick a simple app body depending on target if present
	if target == 'stm32f4'
		app_sources = common + ['app_stm32f4.c']
	elif target == 'sim'
		app_sources = common + ['app_sim.c']
	else
		# generic example
		app_sources = common + ['app_sim.c']
	endif
endif

if app_sources.length() > 0
	# Inherit root include dirs to find wolfboot headers and generated target.h
	incs = [include_directories('.'), inc_dirs, build_inc_dirs]
	# Build position-independent ELF that links against nothing (demo only)
	app_c_args = []
	if 'app_sim.c' in app_sources
		app_c_args += ['-DTARGET_sim']
		# Provide HAL sim implementation for hal_init
		app_sources += ['../hal/sim.c']
	endif
	image_elf = executable('image',
		app_sources,
		include_directories: incs,
		c_args: app_c_args,
		dependencies: wolfboot_dep,
		install: false,
		name_suffix: 'elf',
	)

	objcopy = find_program('objcopy', required: true)
	image_bin_tgt = custom_target('image.bin',
		input: image_elf,
		output: 'image.bin',
		command: [objcopy, '-O', 'binary', '@INPUT@', '@OUTPUT@'],
		build_by_default: false,
	)

	# Expose to top-level as an array for easier checks upstream
	test_app_image_bin = [image_bin_tgt]
	set_variable('test_app_image_bin', test_app_image_bin)

	# Signing and packaging (non-library mode)
	if not meson.is_subproject() and get_option('target') != 'library'
		sign = get_option('sign')
		build_keytools = get_option('build_keytools')
		private_key = get_option('private_key')
		secondary_private_key = get_option('secondary_private_key')
		boot_img_opt = get_option('boot_img')

		# Prefer the just-built image; fallback to user-provided path
		boot_img_inputs = [image_bin_tgt]
		boot_img_cmd_arg = '@INPUT@'

		# Build signed image (image_v1_signed.bin)
		if sign != 'NONE' and build_keytools and keytools_target.length() >= 2
			sign_exe = keytools_target[1]
			img_signed = custom_target(
				'image_v1_signed.bin',
				input: boot_img_inputs,
				output: 'image_v1_signed.bin',
				command: [
					sign_exe,
					boot_img_cmd_arg,
					private_key,
					secondary_private_key != '' ? secondary_private_key : '1',
					'1',
					'--out', '@OUTPUT@',
				],
				build_by_default: false,
				console: true,
			)
		else
			# SIGN=NONE -> copy (use fallback if user provided a path)
			cp = find_program('cp')
			img_signed = custom_target(
				'image_v1_signed.bin',
				input: fs.exists(boot_img_opt) ? [] : boot_img_inputs,
				output: 'image_v1_signed.bin',
				command: [cp, fs.exists(boot_img_opt) ? boot_img_opt : boot_img_cmd_arg, '@OUTPUT@'],
				build_by_default: false,
			)
		endif

		# Packaging with bin-assemble
		if build_bin_assemble and bin_assemble_target.length() > 0
			binasm = bin_assemble_target
			factory = custom_target(
				'factory.bin',
				output: 'factory.bin',
				command: [
					binasm,
					'@OUTPUT@',
					get_option('wolfboot_origin'), 'wolfboot.bin',
					get_option('wolfboot_partition_boot_address'), 'image_v1_signed.bin',
				],
				depends: [wolfboot_bin_tgt, img_signed],
				build_by_default: false,
				console: true,
			)

			internal_flash = custom_target(
				'internal_flash.dd',
				output: 'internal_flash.dd',
				command: [
					binasm,
					'@OUTPUT@',
					'0', 'wolfboot.bin',
					get_option('wolfboot_partition_boot_address'), 'image_v1_signed.bin',
					get_option('wolfboot_partition_update_address'), '/tmp/swap',
					get_option('wolfboot_partition_swap_address'), '/tmp/swap',
				],
				depends: [wolfboot_bin_tgt, img_signed],
				build_by_default: false,
				console: true,
			)
		endif
	endif
endif

