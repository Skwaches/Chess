cmake_minimum_required(VERSION 4.1.2)
project(chess)

find_package(SDL3)
find_package(SDL3_mixer)
find_package(SDL3_image) 

set(srcName src)
set(libraryName library)
set(libName lib)
set(linksName links)

set(exeName ${PROJECT_NAME})
add_executable(${exeName} ${srcName}/main.c)
add_subdirectory(${srcName})
set(requiredLibs SDL3::SDL3 SDL3_mixer::SDL3_mixer SDL3_image sqlite3)
target_link_libraries(${exeName} PRIVATE ${requiredLibs} Custom)
