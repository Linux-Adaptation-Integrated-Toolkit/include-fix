set(LIBRARY_NAME walipkg)
set(CMAKE_CXX_STANDARD 11)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    ADD_DEFINITIONS(-D_DEBUG)
endif()

# 这里通过运行包管理器的命令获取返回值来判定是否存在，通常情况下，当返回值为0时表示存在。
execute_process(COMMAND apt -v RESULT_VARIABLE APT_RES)
if(NOT ${APT_RES})
    message("Find apt in your system, We will Build for Debian.")
    add_definitions(-D_APT)
endif()

set(LIB_SOURCE
        WaliPkgCmd.h
        WaliPkg.h
        WaliPkgCmd.cpp
        WaliPkg.cpp
        log.h
        log.cpp
        WaliRepo.cpp
        WaliRepo.h
        WaliSchedule.cpp
        WaliSchedule.h
        WaliSrcPkg.cpp
        WaliSrcPkg.h)

add_library(${LIBRARY_NAME} ${LIB_SOURCE})

target_link_libraries(${LIBRARY_NAME}
        packagekit-glib2
        gobject-2.0
        glib-2.0
        )
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.4)
        message(FATAL_ERROR "You are on an extremely old version of GCC. Please update your compiler to at least GCC 5.4, preferably latest")
    elseif (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.0)
        message(WARNING "Old Verison of GCC detected. Using Legacy C++ support")
        target_link_libraries(${LIBRARY_NAME} -lstdc++fs)
        target_compile_definitions(${LIBRARY_NAME} PUBLIC LEGACY_CXX)
    endif()
endif()