# Cortex CPU detection module for wolfBoot
# This module provides reusable Cortex-M CPU detection functionality

# Detect Cortex-M CPU variant from cross-compilation settings
cortex_cpu_detected = ''

# First try to get from host machine CPU setting
if meson.is_cross_build()
  host_cpu = meson.get_external_property('cpu', '')
  if host_cpu != ''
    cortex_cpu_detected = host_cpu
  endif
endif

# If not found, try to extract from compiler args
if cortex_cpu_detected == ''
  c_args_from_cross = meson.get_external_property('c_args', [])
  foreach arg : c_args_from_cross
    if arg.startswith('-mcpu=cortex-m')
      cortex_cpu_detected = arg.split('=')[1]
      break
    endif
  endforeach
endif

# Also check built-in options for c_args
if cortex_cpu_detected == ''
  builtin_c_args = get_option('c_args')
  foreach arg : builtin_c_args
    if arg.startswith('-mcpu=cortex-m')
      cortex_cpu_detected = arg.split('=')[1]
      break
    endif
  endforeach
endif
