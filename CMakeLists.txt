cmake_minimum_required(VERSION 3.13)
project(list_desktop C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS -rdynamic)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

# Add libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

ExternalProject_Add(cJSON
        URL https://github.com/DaveGamble/cJSON/archive/master.zip
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/cJSON
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> -DBUILD_SHARED_LIBS=OFF
        )

# Setup dependencies
add_library(dependencies STATIC IMPORTED)
add_dependencies(dependencies PUBLIC cJSON)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

ExternalProject_Get_Property(cJSON install_dir)
include_directories(${install_dir}/include)
set_property(TARGET dependencies PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libcjson.a)
set(CJSON_DIR "${install_dir}")

# Add GResources
add_custom_target(
    resources
    COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/resources && glib-compile-resources --generate-source --target ../resources.c resources.xml
)

# Project build
add_executable(list_desktop main.c resources.c)
add_dependencies(list_desktop resources)
target_link_libraries(list_desktop dependencies ${GTK3_LIBRARIES})
