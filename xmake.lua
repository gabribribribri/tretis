add_rules("mode.debug", "mode.release")
set_languages("c++23")

toolchain("myclang")
  set_kind("standalone")
  set_toolset("cxx", "clang++")
  set_toolset("ld", "clang++")

target("tretis")
  set_kind("binary")
  set_toolchains("myclang")
  add_files("src/*.cpp")
  add_links("sfml-graphics", "sfml-window", "sfml-system")
  set_warnings("all", "extra")
  add_cxxflags("-stdlib=libc++")
  add_ldflags("-stdlib=libc++", "-Wl,-rpath,/usr/lib")
  set_policy("run.autobuild", true)
  if is_mode("release") then 
    set_optimize("fastest")
  end
  if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
  end
