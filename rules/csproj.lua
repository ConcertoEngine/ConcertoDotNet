rule("csproj")
	set_extensions(".csproj", ".cs")

    on_config(function (target)
        for _, filepath in ipairs(target:sourcebatches()["csproj"].sourcefiles) do
            target:add("extrafiles", filepath, {always_added = true})
        end
    end)

	on_build_file(function (target, sourcefile)
        if path.extension(sourcefile) == ".csproj" then
            if has_config("corehost") then
                os.execv(path.join(get_config("corehost"), "dotnet.exe"), {"build", sourcefile, "-o", target:targetdir(), "--artifacts-path", target:targetdir()})
            else
                os.execv("dotnet", {"build", sourcefile, "-o", target:targetdir(), "--artifacts-path", target:targetdir()})
            end
        end
	end)

    after_build(function(target)
        local binaryPath = "$(buildir)/$(plat)/$(arch)/$(mode)"
        os.cp("Sample/DotNetLib.deps.json", binaryPath)
        os.cp("Sample/DotNetLib.runtimeconfig.json", binaryPath)
    end)

rule_end()