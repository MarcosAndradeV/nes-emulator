project('nes-emulator', 'cpp')

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
  'src/core/Main.cpp',
)

executable('emulator',
  sources: sources,
  include_directories: inc_dir,
  dependencies: dependencies
)
