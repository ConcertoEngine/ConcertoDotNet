rule("csproj")
	set_extensions(".csproj")
	on_build_file(function (target, sourcefile)
		os.execv("dotnet", {"build", sourcefile, "-o", target:targetdir()})
	end)

    after_build(function(target)
        local binaryPath = "$(buildir)/$(plat)/$(arch)/$(mode)"
        -- os.cp("DotNetLib.deps.json", binaryPath)
        -- os.cp("DotNetLib.runtimeconfig.json", binaryPath)
    end)

rule_end()


target("concerto-dotnet-sample")
    set_kind("binary")
    set_languages("cxx20")
    set_warnings("all")

    add_headerfiles("DotNetHello.hpp")
    add_includedirs(".")

    add_files("DotNetHello.cpp", "DotNetPackage.xml")
    add_files("DotNetLib.csproj")

    add_deps("concerto-dotnet")
    add_packages("concerto-reflection")

    add_rules("csproj")
    add_rules("xml_reflect")