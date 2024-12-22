package("dotnetdebug")
    set_homepage("https://dotnet.microsoft.com/")
    set_description(".NET is the free, open-source, cross-platform framework for building modern apps and powerful cloud services.")

    on_fetch(function (package)
        local result = {}
        result.links = "nethost"
        result.linkdirs = "E:/Documents/git/dotnet/runtime/artifacts/bin/win-x64.Debug/corehost"
        result.includedirs = "E:/Documents/git/dotnet/runtime/artifacts/bin/win-x64.Debug/corehost"
        return result
    end)

    on_install("windows|x86", "windows|x64", "windows|arm64", "linux|x64", "linux|x86_64", "linux|arm64", "linux|arm64-v8a", "macosx|x86_64", "macosx|x64", "macosx|arm64", function (package)
        local version_str = package:version_str()
        local out_path = "packs"
        if package:is_plat("windows") then
            local arch = package:is_arch("x64") and "x64" or (package:is_arch("arm64") and "arm64" or "x86")
            out_path = path.join(out_path, "Microsoft.NETCore.App.Host.win-" .. arch, version_str, "runtimes", "win-" .. arch, "native")

            os.cp(path.join(out_path, "nethost.dll"), package:installdir("bin"))
            os.cp(path.join(out_path, "nethost.lib"), package:installdir("lib"))
        elseif package:is_plat("linux") then
            local arch = package:is_arch("arm64", "arm64-v8a") and "arm64" or "x64"
            out_path = path.join(out_path, "Microsoft.NETCore.App.Host.linux-" .. arch, version_str, "runtimes", "linux-" .. arch, "native")

            if package:config("shared") then
                os.cp(path.join(out_path, "*.so"), package:installdir("lib"))
            else
                os.cp(path.join(out_path, "*.a"), package:installdir("lib"))
            end
        elseif package:is_plat("macosx") then
            local arch = package:is_arch("arm64") and "arm64" or "x64"
            out_path = path.join(out_path, "Microsoft.NETCore.App.Host.osx-" .. arch, version_str, "runtimes", "osx-" .. arch, "native")

            if package:config("shared") then
                os.cp(path.join(out_path, "*.dylib"), package:installdir("lib"))
            else
                os.cp(path.join(out_path, "*.a"), package:installdir("lib"))
            end
        end

        os.cp(path.join(out_path, "*.h"), package:installdir("include"))
        package:add("links", "nethost")

    end)

    on_test(function (package)
        assert(package:has_cfuncs("get_hostfxr_path", {includes = "nethost.h"}))
    end)
package_end()

add_rules("mode.debug", "mode.release")
add_repositories("Concerto-xrepo https://github.com/ConcertoEngine/xmake-repo.git main")
add_requires("dotnetdebug", "semver", "gtest", "concerto-core", "concerto-reflection", {configs = {debug = is_mode("debug"), with_symbols = true}})


option("hostfxr", {description = "Path to the hostfxr library", default = "", type = "string"})

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
    if has_config("hostfxr") then
        add_defines("CCT_HOSTFXR_PATH=\"" .. get_config("hostfxr") .. "\"")
    end

includes("Rules/*.lua")
includes("Tests/xmake.lua")
includes("Sample/xmake.lua")