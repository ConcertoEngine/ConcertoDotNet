
target("concerto-dotnet-sample")
    set_kind("binary")
    set_languages("cxx20")
    set_warnings("all")

    add_headerfiles("DotNetHello.hpp")
    add_includedirs(".")

    add_files("DotNetHello.cpp", "DotNetLibPackage.xml")
    add_files("DotNetLib.csproj", "*.cs")

    add_deps("concerto-dotnet")
    add_packages("concerto-reflection")

    add_rules("csproj")
    add_rules("@concerto-reflection/xml_reflect")