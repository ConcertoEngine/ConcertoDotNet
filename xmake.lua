add_rules("mode.debug", "mode.release")
add_repositories("Concerto-xrepo https://github.com/ConcertoEngine/xmake-repo.git main")
add_requires("dotnet", "gtest", "concerto-core", "concerto-reflection", {configs = {debug = is_mode("debug"), with_symbols = true}})

if is_plat("windows") then
    set_runtimes(is_mode("debug") and "MDd" or "MD")
end

target("concerto-dotnet")
    set_languages("cxx20")
    set_kind("shared")
    set_warnings("all")

    add_defines("CCT_DOTNET_BUILD")
    add_packages("dotnet", "concerto-core", {public = true})
    add_files("Src/*.cpp")
    add_cxxflags("cl::/Zc:preprocessor", { public = true })
    add_includedirs('Include',
                    'Include/Concerto',
                    'Include/Concerto/DotNet', {public = true})

    add_headerfiles('Include/(Concerto/DotNet/*.hpp)')

includes("rules/**.lua")
includes("Tests/xmake.lua")
includes("Sample/xmake.lua")