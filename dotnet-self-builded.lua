package("dotnet-self-builded")
    set_homepage("https://dotnet.microsoft.com/")
    set_description(".NET is the free, open-source, cross-platform framework for building modern apps and powerful cloud services.")

    add_configs("corehost", {description = "Path to the corehost library", default = "", type = "string"})

    on_fetch(function (package)
        local result = {}
        if package:config("corehost") then
            result.links = "nethost"
            result.linkdirs = package:config("corehost")
            result.includedirs = package:config("corehost")
        end
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
