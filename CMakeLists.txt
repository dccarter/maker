cmake_minimum_required(VERSION 3.14)
project(lush VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)


include(LushLua2C)
include(LushGetOsVariant)
LushGetOsVariant(OS_VARIANT)

set(LUSH_SCRIPTS
        lush/main.lua
        lush/sys/argparse.lua
        lush/sys/http.lua
        lush/sys/json.lua
        lush/sys/logger.lua
        lush/sys/sh.lua
        lush/sys/utils.lua
        lush/sys/show.lua)

LushLua2C(lush
        SCRIPTS ${LUSH_SCRIPTS})

find_package(LuaS REQUIRED)
find_package(Dl)

add_executable(lush
        ${lush_LUA2C_SOURCES}
        src/lush.cpp
        src/main.cpp)
target_link_libraries(lush LuaS::Lua Dl::Dl)

target_compile_definitions(lush PUBLIC LUSH_VERSION=\"${CMAKE_PROJECT_VERSION}\")
target_compile_definitions(lush PUBLIC SRC_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")
target_compile_definitions(lush PUBLIC OS_VARIANT=\"${OS_VARIANT}\")

include_directories(${CMAKE_BINARY_DIR}/.generated)
include_directories(include)

install(TARGETS lush DESTINATION bin)

if (OS_VARIANT)
    set(CPACK_PACKAGE_NAME   ${PROJECT_NAME})
    set(CPACK_PACKAGE_VENDOR "Suilteam")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Carter Mbotho")
    set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION})
    set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${OS_VARIANT}-${CMAKE_PROJECT_VERSION})
    set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
    set(CPACK_GENERATOR "TGZ")
    include(CPack)
endif()