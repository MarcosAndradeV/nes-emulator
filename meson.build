project('nes-emulator', 'cpp',
    version: '0.1.0',
    license: 'MIT',
    license_files : 'LICENSE',
    default_options : ['warning_level=3']
)

cxx = meson.get_compiler('cpp')

dependencies = []
current_source_dir = meson.current_source_dir()

if host_machine.system() == 'windows'
  dependencies += [
    cxx.find_library('winmm'),
  ]
elif host_machine.system() == 'darwin'
  link_args += [
    '-framework', 'AppKit',
    '-framework', 'IOKit',
  ]
elif host_machine.system() == 'linux'
  dependencies += [
    cxx.find_library('m'),
    cxx.find_library('dl'),
    cxx.find_library('raylib', dirs: [
      join_paths(current_source_dir, 'third_party/Raylib/raylib-5.0_linux_amd64/lib')
    ]),
  ]
endif

inc_dir = include_directories(
  'include',
)

sources = files(
    'src/Main.cpp',
    'src/Source_files/Emulator.cpp',
    'src/Source_files/Bus.cpp',
    'src/Source_files/CPU.cpp',
    'src/Source_files/PPU.cpp',
    'src/Source_files/APU.cpp',
    'src/Source_files/Cartridge.cpp',
    'src/Mappers/Mapper.cpp',
)

executable('emulator',
  sources: sources,
  include_directories: inc_dir,
  dependencies: dependencies
)

# test_sources = files(
#   './tests/TestPpu.cpp',
#   './tests/TestCpu.cpp'
# )

test_dir = 'tests'