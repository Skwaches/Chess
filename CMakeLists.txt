cmake_minimum_required(VERSION 4.1.2)
project(chess)

find_package(SDL3)
find_package(SDL3_mixer)
find_package(SDL3_image) 

set(exeName ${PROJECT_NAME})

add_executable(${exeName} src/main.c)

file(GLOB Libs src/Lib/*.c)
add_library(Custom SHARED ${Libs})
target_include_directories(Custom INTERFACE Include)

set(requiredLibs Custom SDL3::SDL3 SDL3_mixer::SDL3_mixer SDL3_image sqlite3)
target_link_libraries(${exeName} PRIVATE ${requiredLibs})
