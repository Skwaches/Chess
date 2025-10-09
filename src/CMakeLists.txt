file(GLOB libFiles ${libraryName}/${libName}/*.c)
add_library(Custom SHARED ${libFiles})
target_include_directories(Custom INTERFACE ${libraryName}/${linksName})