-- Something here doesn't work as it should...
-- Problem for tomorrow !
-- TODO: README.md
add_rules("mode.debug", "mode.release")
set_languages("c++23")

option("compiler-toolchain")
  set_values("classic", "clang-with-custom-llvm-build")
  set_default("classic")
  set_showmenu(true)
  set_description("Set the Compiler and the standard library to be used")

toolchain("clang-with-custom-llvm-build")
  set_kind("standalone")
  set_toolset("cxx", "clang++")
  set_toolset("ld", "clang++")
  add_cxxflags("-stdlib=libc++")
  add_ldflags("-stdlib=libc++", "-Wl,-rpath,/usr/local/lib/x86_64-unknown-linux-gnu")

toolchain("classic")
  set_kind("standalone")
  set_toolset("cxx", "g++")
  set_toolset("ld", "g++")

target("tretis")
  set_kind("binary")
  set_toolchains("myclang")
  add_files("src/*.cpp")
  add_links("sfml-graphics", "sfml-window", "sfml-system")
  set_warnings("all", "extra")
  if has_config("compiler-toolchain", "clang-with-custom-llvm-build") then
    set_toolchains("clang-with-custom-llvm-build")
  elseif has_config("compiler-toolchain", "classic") then
    set_toolchains("classic")
  end
  set_policy("run.autobuild", true)
  if is_mode("release") then 
    set_optimize("fastest")
  end
  if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
  end
