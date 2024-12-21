add_rules("mode.debug", "mode.release")
add_repositories("Concerto-xrepo https://github.com/ConcertoEngine/xmake-repo.git main")
add_requires("dotnet", "gtest", "concerto-core", "concerto-reflection", {configs = {debug = is_mode("debug"), with_symbols = true}})

if is_plat("windows") then
    set_runtimes(is_mode("debug") and "MDd" or "MD")
end

rule("csproj")
	set_extensions(".csproj", ".cs")

    on_config(function (target)
        for _, filepath in ipairs(target:sourcebatches()["csproj"].sourcefiles) do
            target:add("extrafiles", filepath, {always_added = true})
        end
    end)

	on_build_file(function (target, sourcefile)
        if path.extension(sourcefile) == ".csproj" then
            print(target:targetdir())
            os.execv("dotnet", {"build", sourcefile, "-o", target:targetdir(), "--artifacts-path", target:targetdir()})
        end
	end)

    after_build(function(target)
        local binaryPath = "$(buildir)/$(plat)/$(arch)/$(mode)"
        os.cp("Sample/DotNetLib.deps.json", binaryPath)
        os.cp("Sample/DotNetLib.runtimeconfig.json", binaryPath)
    end)

rule_end()

target("concerto-dotnet")
    set_languages("cxx20")
    set_kind("shared")
    set_warnings("all")

    add_defines("CCT_DOTNET_BUILD")
    add_packages("dotnet", "concerto-core", "concerto-reflection", {public = true})
    add_files("Src/*.cpp", "Src/*.xml")
    add_cxxflags("cl::/Zc:preprocessor", { public = true })
    add_includedirs('Include',
                    'Include/Concerto',
                    'Include/Concerto/DotNet', {public = true})

    add_rules("xml_reflect")
    add_headerfiles('Include/(Concerto/DotNet/*.hpp)')


includes("Tests/xmake.lua")
includes("Sample/xmake.lua")