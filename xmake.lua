add_rules("mode.debug", "mode.release")
set_languages("c++23")

target("tretis")
  set_kind("binary")
  add_files("src/*.cpp")
  set_warnings("all", "extra")
  add_links("sfml-graphics", "sfml-window", "sfml-system")
  set_policy("run.autobuild", true)
  if is_mode("release") then
    set_optimize("fastest")
  end
  if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
  end
