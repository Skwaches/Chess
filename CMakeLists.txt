cmake_minimum_required(VERSION 4.1.2)
project(chess)

#Set output directory for built objects
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/library")

# File for lsp
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(executable ${PROJECT_NAME})

#Libraries location 
#CMAKE_CURRENT_SOURCE_DIR is where the CMakeLists.txt file is.
option(VENDORED "Use personal libraries" ON)
set(VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../.Vendor)

if(VENDORED)
	message("Using vendored libraries")
	add_subdirectory(${VENDOR_DIR}/SDL SDL)
	add_subdirectory(${VENDOR_DIR}/SDL_mixer SDL_mixer)
	add_subdirectory(${VENDOR_DIR}/SDL_image SDL_image)
	find_package(SQLite3 REQUIRED)
else()
	find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3-shared)
	find_package(SDL3_mixer REQUIRED)
	find_package(SDL3_image REQUIRED) 
	find_package(SQLite3 REQUIRED)
endif()
unset(VENDORED CACHE)

#Add library and it's dependencies
file(GLOB libs ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/*.c)
add_library(library ${libs})
target_link_libraries( library PUBLIC 
	SDL3::SDL3
	SDL3_mixer::SDL3_mixer
	SDL3_image::SDL3_image
	sqlite3
)
# Guide include search.
target_include_directories(library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/include)

#Link library to executable.
add_executable(${executable} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c)
target_link_libraries(${executable} PRIVATE library)
