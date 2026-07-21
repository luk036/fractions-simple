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
