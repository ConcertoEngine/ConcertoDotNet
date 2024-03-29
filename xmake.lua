package('nethost')
    set_homepage('https://github.com/dotnet/runtime')
    set_description('.NET is a cross-platform runtime for cloud, mobile, desktop, and IoT apps.')
    set_license('MIT')

    add_urls('https://github.com/dotnet/runtime.git')

    add_versions('v7.0.0', 'a2bd895b471dea0bdad35436054abdf8a7ea4fd7df25cdeaf5325adbbd4c5baa')
    add_patches('v7.0.0',  "https://raw.githubusercontent.com/microsoft/vcpkg/master/ports/nethost/0001-nethost-cmakelists.patch", '6c4974f9fe61e4fee5000f60790dd462d1ff2e07f558814aeb0b63c23bddb305')

    add_versions('v8.0.0-preview', 'c7537bdf261a7160836447ec46e1f136580a9fe4')
    add_patches('v8.0.0-preview', "https://raw.githubusercontent.com/microsoft/vcpkg/master/ports/nethost/0001-nethost-cmakelists.patch", '6c4974f9fe61e4fee5000f60790dd462d1ff2e07f558814aeb0b63c23bddb305')

    if is_host('windows') then
        set_policy('platform.longpaths', true)
    end

    on_install(function (package)
        local configs = {}
        os.mkdir((package:cachedir() .. '/source/nethost/build'))
        table.insert(configs, '-S ' .. package:cachedir() .. '/source/nethost/src/native/corehost/nethost/')
        table.insert(configs, '-B ' .. package:cachedir() .. '/source/nethost/build')
        table.insert(configs, '-DSKIP_VERSIONING=1')
        table.insert(configs, '-DCLR_CMAKE_HOST_ARCH=' .. (package:is_arch('x64') and 'x64' or 'x86'))
        table.insert(configs, '-DCLI_CMAKE_HOST_POLICY_VER=' .. package:version_str())
        table.insert(configs, '-DCLI_CMAKE_HOST_FXR_VER=' .. package:version_str())
        table.insert(configs, '-DCLI_CMAKE_HOST_VER=' .. package:version_str())
        table.insert(configs, '-DCLI_CMAKE_COMMON_HOST_VER=' .. package:version_str())
        table.insert(configs, '-DCLI_CMAKE_PKG_RID=' .. (package:is_arch('x64') and 'win-x64' or 'win-x86'))
        table.insert(configs, '-DCLI_CMAKE_COMMIT_HASH=' .. package:version_str():sub(2))
        table.insert(configs, "-DCLR_CMAKE_TARGET_ARCH_" .. package:plat() .. "=1")
        table.insert(configs, "-DCLR_CMAKE_TARGET_ARCH=" .. (package:is_arch('x64') and 'x64' or 'x86'))
        table.insert(configs, "-DCLR_CMAKE_HOST_ARCH=" .. (package:is_arch('x64') and 'x64' or 'x86'))
        table.insert(configs, "-DCLI_CMAKE_FALLBACK_OS=" .. package:plat())
        table.insert(configs, "-DNETHOST_EXPORT=1")
        table.insert(configs, "-DBUILD_SHARED_LIBS=1")
        import('package.tools.cmake').install(package, configs, {buildir = (package:cachedir() .. '/source/nethost/build')})
    end)
package_end()

package('dylib')
    set_kind("library", {headeronly = true})
    set_homepage("https://github.com/martin-olivier/dylib")
    set_description("C++ cross-platform wrapper around dynamic loading of shared libraries (dll, so, dylib)")
    set_license("MIT")
    add_urls("https://github.com/martin-olivier/dylib/archive/refs/tags/$(version).tar.gz")
    add_versions("v2.1.0", 'f48b68549b93aeb794d003793329f82e82b193dda26797c23efb3a5e83d4e33b')

    on_install("windows", "linux", "mingw", "macosx", function (package)
        os.cp("include/dylib.hpp", package:installdir("include"))
    end)
package_end()

add_rules("mode.debug")
add_requires("nethost v8.0.0-preview", 'dylib', 'gtest')

target('ConcertoDotNet')
    set_languages("cxx20")
    set_kind("binary")
    set_warnings("all")
    add_packages("nethost", 'dylib')
    add_files("Src/*.cpp")
    add_includedirs("Include")
    set_optimize("fastest")
    after_build(function(target)
        print("Copying resources...")
        local binaryPath = "$(buildir)/$(plat)/$(arch)/$(mode)"
        os.cp("Assets/**", binaryPath)
        print("Copying resources... Done !")
    end)

target('ConcertoDotNetTests')
    set_kind("binary")
    set_languages("cxx20")
    set_warnings("all")
    add_packages("nethost", 'dylib', 'gtest')
    add_files("Tests/*.cpp")
    add_files("Src/HostFXR.cpp")
    add_includedirs("Include")
    after_build(function(target)
        print("Copying resources...")
        local binaryPath = "$(buildir)/$(plat)/$(arch)/$(mode)"
        os.cp("Assets/**", binaryPath)
        print("Copying resources... Done !")
    end)
