
add_rules("mode.debug", "mode.release")
add_repositories("Concerto-xrepo https://github.com/ConcertoEngine/xmake-repo.git main")

---- Options ----
option("dotnet_self_builded", {description = "Use a provided build", default = false, type = "boolean"})
option("hostfxr", {description = "Path to the hostfxr library", default = nil, type = "string"})
option("corehost", {description = "Path to the corehost folder", default = nil, type = "string"})

if has_config("dotnet_self_builded") then
    includes("dotnet-self-builded.lua")
    add_requires("dotnet-self-builded", {configs = {corehost = get_config("corehost")}})
else
    add_requires("dotnet")
end

add_requires("semver", "gtest", "concerto-core", "concerto-reflection", {configs = {debug = is_mode("debug"), with_symbols = true}})

if is_plat("windows") then
    set_runtimes(is_mode("debug") and "MDd" or "MD")
end

target("concerto-dotnet")
    set_languages("cxx20")
    set_kind("shared")
    set_warnings("all")
    if (is_mode("debug")) then
        set_symbols("debug")
    end

    add_defines("CCT_DOTNET_BUILD")
    add_packages("dotnetdebug", "semver", "concerto-core", "concerto-reflection", {public = true})
    add_files("Src/*.cpp", "Src/*.xml")
    add_cxxflags("cl::/Zc:preprocessor", { public = true })
    add_includedirs('Include',
                    'Include/Concerto',
                    'Include/Concerto/DotNet', {public = true})

    add_rules("xml_reflect")
    add_headerfiles('Include/(Concerto/DotNet/*.hpp)')

    if has_config("dotnet_self_builded") then
        add_defines("CCT_HOSTFXR_PATH=\"" .. get_config("hostfxr") .. "\"")
        add_packages("dotnet-self-builded", {public = true})
    else
        add_packages("dotnet", {public = true})
    end

includes("Rules/*.lua")
includes("Tests/xmake.lua")
includes("Sample/xmake.lua")