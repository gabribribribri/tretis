add_rules("mode.debug", "mode.release")
add_requires("sfml 2.6.1")

local font_name = "./apercumovistarbold.ttf"


target("tretis")
  --- BASIC CONFIG ---
  set_kind("binary")
  add_files("src/*.cpp")
  set_languages("c++26")
  add_packages("sfml")
  set_toolchains("clang")
  set_warnings("all", "extra")
  
  --- POLICIES ---
  set_policy("run.autobuild", true)
  set_policy("generator.compile_commands", true)
  add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})
  
  --- SANITIZER ---
  -- set_policy("build.sanitizer.undefined", true)
  -- set_policy("build.sanitizer.memory", true)
  -- set_policy("build.sanitizer.leak", true)
  -- set_policy("build.sanitizer.address", true)

  --- BUILD MODES ---
  if is_mode("release") then
    set_optimize("fastest")
    set_policy("build.optimization.lto", true)
  end
  if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
  end

  --- COPYING ASSETS AFTER BUILD ---
  after_build(function(target) -- Copying assets
    os.cp(font_name, target:targetdir())
  end)
