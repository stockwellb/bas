set_project("bas")
set_languages("c11")

-- Single-file libraries live at the repo root; tests include them from there.
add_includedirs("$(projectdir)")
add_syslinks("m")

-- Each test is a single translation unit, so it is the one place the
-- single-header implementations get compiled. Defining these here (rather than
-- with an in-source #define) also lands them in compile_commands.json, so
-- clangd stops greying out the #ifdef ..._IMPLEMENTATION sections.
add_defines("BAS_GEOMETRY_IMPLEMENTATION", "BAS_ARENA_IMPLEMENTATION", "BAS_TEST_IMPLEMENTATION")

local tests = {"scalar", "vec2", "aabb", "arena"}

for _, name in ipairs(tests) do
    target("test_" .. name)
        set_kind("binary")
        set_default(false)
        add_files("tests/test_" .. name .. ".c")
        add_tests(name)
end

-- Custom task: `xmake readme` regenerates the README library table. The task
-- declaration lives here; the pure generation logic lives in tasks/readme.lua.
task("readme")
    set_menu {
        usage = "xmake readme",
        description = "Regenerate the library table in README.md from header title lines"
    }
    on_run(function ()
        local gen = import("readme", {rootdir = path.join(os.projectdir(), "tasks")})

        local headers = {}
        for _, header in ipairs(os.files(path.join(os.projectdir(), "bas_*.h"))) do
            table.insert(headers, {name = path.filename(header), content = io.readfile(header) or ""})
        end

        local readme_path = path.join(os.projectdir(), "README.md")
        local content = io.readfile(readme_path)
        assert(content, "readme: README.md not found")

        io.writefile(readme_path, gen.render(headers, content))
        cprint("${green}readme: regenerated README library table")
    end)
