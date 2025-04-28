project(
  'nes-emulator',
  'cpp',
  version: '0.1.0',
  license: 'MIT',
  license_files: 'LICENSE',
  default_options: ['warning_level=3', 'cpp_std=c++17'],
)

cxx = meson.get_compiler('cpp')

dependencies = []
current_source_dir = meson.current_source_dir()

if host_machine.system() == 'windows'
  dependencies += [
    cxx.find_library('winmm'),
    cxx.find_library(
      'tinyfiledialogs64',
      dirs: [
        join_paths(
          current_source_dir,
          'third_party/tinyfiledialogs_lib/dll_cs_lua_R_fortran_pascal/',
        ),
      ],
    ),
  ]
elif host_machine.system() == 'darwin'
  # TODO Add tiny file dialogs to dependencies
  link_args += [
    '-framework', 'AppKit',
    '-framework', 'IOKit',
  ]
elif host_machine.system() == 'linux'
  dependencies += [
    cxx.find_library('m'),
    cxx.find_library('dl'),
    cxx.find_library(
      'raylib',
      dirs: [
        join_paths(current_source_dir, 'third_party/Raylib/raylib-5.0_linux_amd64/lib'),
      ],
    ),
    cxx.find_library(
      'tinyfiledialogsLinux64',
      dirs: [
        join_paths(
          current_source_dir,
          'third_party/tinyfiledialogs_lib/dll_cs_lua_R_fortran_pascal/',
        ),
      ],
    ),
  ]
endif

inc_dir = include_directories('include')

# Arquivos principais do emulador
emulator_sources = files(
  'src/Main.cpp',
  'src/Mappers/Mapper.cpp',
  'src/Mappers/Mapper000.cpp',
  'src/Mappers/Mapper001.cpp',
  'src/Mappers/Mapper002.cpp',
  'src/Mappers/Mapper003.cpp',
  'src/Mappers/Mapper004.cpp',
  'src/Mappers/Mapper066.cpp',
  'src/Mappers/Mapper005.cpp',
  'src/Mappers/Mapper007.cpp',
  'src/Source_files/APU.cpp',
  'src/Source_files/Bus.cpp',
  'src/Source_files/CPU.cpp',
  'src/Source_files/Cartridge.cpp',
  'src/Source_files/Emulator.cpp',
  'src/Source_files/PPU.cpp',
  'src/Source_files/UI.cpp',
)

# Executável principal do emulador
executable(
  'emulator',
  emulator_sources,
  include_directories: inc_dir,
  dependencies: dependencies,
  install: true,
  install_dir: 'build',
)

# Arquivos de teste
test_sources = files(
  'tests/CPUTest.cpp',
  'src/Mappers/Mapper.cpp',
  'src/Mappers/Mapper000.cpp',
  'src/Mappers/Mapper001.cpp',
  'src/Mappers/Mapper002.cpp',
  'src/Mappers/Mapper003.cpp',
  'src/Mappers/Mapper004.cpp',
  'src/Mappers/Mapper066.cpp',
  'src/Mappers/Mapper005.cpp',
  'src/Mappers/Mapper007.cpp',
  'src/Source_files/APU.cpp',
  'src/Source_files/Bus.cpp',
  'src/Source_files/CPU.cpp',
  'src/Source_files/Cartridge.cpp',
  'src/Source_files/Emulator.cpp',
  'src/Source_files/PPU.cpp',
  'src/Source_files/UI.cpp',
)

# Executável de teste
TestCPU = executable(
  'TestCPU',
  test_sources,
  include_directories: inc_dir,
  dependencies: dependencies,
)

# Registrar o teste
test('cpu_test', TestCPU)