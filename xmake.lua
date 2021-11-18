add_rules("mode.debug", "mode.release")

add_requires("lz4", "dpkg")

target("libwali-search")
    set_kind("static")
    add_files("src/*.cpp")
    add_packages("lz4", "dpkg")
    add_syslinks("stdc++fs")

target("demo")
    set_kind("binary")
    add_files("example/wali-afs-demo.cpp")
    add_deps("libwali-search")
    add_includedirs("src")