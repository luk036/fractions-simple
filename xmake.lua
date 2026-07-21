add_rules("mode.debug", "mode.release")
set_languages("c++17")
set_warnings("all", "error")

if is_plat("windows") then
    add_cxflags("/utf-8")
end

-- mode.coverage adds --coverage which only GCC/Clang support
if not is_plat("windows") then
    add_rules("mode.coverage")
end

add_requires("doctest")

target("fraction")
    set_kind("headeronly")
    add_headerfiles("include/(fraction/**.hpp)")
    add_includedirs("include", {public = true})

target("test_fraction")
    set_kind("binary")
    add_deps("fraction")
    add_files("tests/test_fraction.cpp")
    add_includedirs("include")
    add_packages("doctest")

-- ---------------------------------------------------------------------------
-- Format tasks (clang-format + cmake-format)
-- ---------------------------------------------------------------------------

task("format")
    set_category("format")
    set_menu {
        usage = "xmake format [options]",
        description = "Check C++ and CMake formatting",
        options = {
            { 'a', 'apply', "k", nil, "Apply formatting fixes in-place" }
        }
    }
    on_run(function()
        local apply = has_config("apply")
        local sources = {
            "include/fraction/fraction.hpp",
            "tests/test_fraction.cpp",
            "CMakeLists.txt",
            "xmake.lua",
            "cmake/CPM.cmake",
        }
        local ok = true
        for _, src in ipairs(sources) do
            if os.isfile(src) then
                if src:match("%.hpp$") or src:match("%.cpp$") then
                    if apply then
                        os.exec("clang-format -i -style=file " .. src)
                        print("  fixed: " .. src)
                    else
                        local formatted, err = os.iorun("clang-format -style=file " .. src)
                        local f = io.open(src, "r")
                        local original = f:read("*a")
                        f:close()
                        if formatted ~= original then
                            ok = false
                            print("  needs formatting: " .. src)
                        end
                    end
                elseif src:match("%.cmake$") or src == "CMakeLists.txt" then
                    if apply then
                        os.exec("cmake-format -i " .. src)
                        print("  fixed: " .. src)
                    else
                        local ok2, err = os.iorun("cmake-format --check " .. src)
                        if not ok2 then
                            ok = false
                            print("  needs formatting: " .. src)
                        end
                    end
                end
            end
        end
        if not apply and not ok then
            os.exit(1)
        end
    end)

task("fix-format")
    set_category("format")
    set_menu {
        usage = "xmake fix-format",
        description = "Apply formatting fixes in-place",
    }
    on_run(function()
        local sources = {
            "include/fraction/fraction.hpp",
            "tests/test_fraction.cpp",
            "CMakeLists.txt",
            "xmake.lua",
            "cmake/CPM.cmake",
        }
        for _, src in ipairs(sources) do
            if os.isfile(src) then
                if src:match("%.hpp$") or src:match("%.cpp$") then
                    os.exec("clang-format -i -style=file " .. src)
                    print("  fixed: " .. src)
                elseif src:match("%.cmake$") or src == "CMakeLists.txt" then
                    os.exec("cmake-format -i " .. src)
                    print("  fixed: " .. src)
                end
            end
        end
    end)
