rule("xml_reflect")
    set_extensions(".xml")
    add_deps("find_cct_pkg_generator")
    on_config(function (target)
        for _, filepath in ipairs(target:sourcebatches()["xml_reflect"].sourcefiles) do
            local generatedFile = path.join(target:autogendir(), "Reflection", path.basename(filepath))
            target:add("headerfiles", path.join(target:autogendir(), "(Reflection", path.basename(filepath) ..".hpp)"))
            target:add("includedirs", path.join(target:autogendir(), "Reflection"), {public = true})
            target:add("files", generatedFile .. ".cpp", {always_added = true})
            target:add("headerfiles", filepath)
            target:add("defines", path.basename(filepath):upper() .. "_BUILD")
        end
    end)

    before_buildcmd_file(function (target, batchcmds, xmlFile, opt)
        local cctPkgGen = target:data("concerto-pkg-generator")
        local envs = target:data("concerto-pkg-generator-envs")
        assert(cctPkgGen, "concerto-pkg-generator not found!")
        local outputFolder = path.join(target:autogendir(), "Reflection")
        local outputCppFile = path.join(outputFolder, path.basename(xmlFile) .. ".cpp")
        local outputHppFile = path.join(outputFolder, path.basename(xmlFile) .. ".hpp")
        local exePrefix = target:is_plat("mingw", "windows") and ".exe" or ""

        batchcmds:show_progress(opt.progress, "${color.build.object}compiling.reflection %s", xmlFile)
        batchcmds:vrunv(cctPkgGen.program, {xmlFile, path.join(target:autogendir(), "Reflection")}, {envs = envs})

        batchcmds:add_depfiles(xmlFile)
        --batchcmds:add_depvalues() todo add version from cabal
        batchcmds:set_depmtime(os.mtime(outputCppFile))
        batchcmds:set_depcache(target:dependfile(outputCppFile))
    end)