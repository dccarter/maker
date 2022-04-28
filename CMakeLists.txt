cmake_minimum_required(VERSION 3.14)
project(maker VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)


include(MakerLua2C)
include(MakerGetOsVariant)
MakerGetOsVariant(OS_VARIANT)

set(MAKER_SCRIPTS
        maker/main.lua
        maker/sys/argparse.lua
        maker/sys/http.lua
        maker/sys/json.lua
        maker/sys/logger.lua
        maker/sys/sh.lua
        maker/sys/utils.lua
        maker/sys/show.lua)

MakerLua2C(maker
        SCRIPTS ${MAKER_SCRIPTS})

find_package(LuaS REQUIRED)
find_package(Dl)

add_executable(maker
        ${maker_LUA2C_SOURCES}
        src/maker.cpp
        src/main.cpp)
target_link_libraries(maker LuaS::Lua Dl::Dl)

target_compile_definitions(maker PUBLIC MAKER_VERSION=\"${CMAKE_PROJECT_VERSION}\")
target_compile_definitions(maker PUBLIC SRC_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")
target_compile_definitions(maker PUBLIC OS_VARIANT=\"${OS_VARIANT}\")

include_directories(${CMAKE_BINARY_DIR}/.generated)
include_directories(include)

install(TARGETS maker DESTINATION bin)

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