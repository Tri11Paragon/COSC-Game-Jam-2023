cmake_minimum_required(VERSION 3.24)
cmake_policy(SET CMP0015 NEW)
project(COSC_Game_Jam_2023)

set(CMAKE_CXX_STANDARD 17)

option(USE_EXTRAS "Use emscriptem" OFF)

if (USE_EXTRAS)
    message("Using emscriptem linker and compile flags!")
    #add_definitions("-std=c++17")
    set(CMAKE_VERBOSE_MAKEFILE on)
else()
    # used to debug memory related issues
    if ((CMAKE_BUILD_TYPE MATCHES Debug))
        if (UNIX)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
            add_compile_options(-fsanitize=address)
            add_link_options(-fsanitize=address)
        endif ()
        message("Building for Debug")
    else ()
        message("Building for Release")
    endif ()

    find_package(SDL2)

    include_directories(${SDL2_INCLUDE_DIRS})

    message("Include: ${SDL2_INCLUDE_DIRS} with ${SDL2_FOUND} and ${SDL2_LIBRARIES}")
endif()


add_subdirectory(libraries/BLT)

file(GLOB_RECURSE files "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

include_directories(include/)

add_executable(COSC_Game_Jam_2023 ${files} include/window.h include/image/image.h src/image/image.cpp src/window.cpp)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

target_link_libraries(COSC_Game_Jam_2023 BLT)

if (USE_EXTRAS)
    set_target_properties(COSC_Game_Jam_2023 PROPERTIES LINK_FLAGS "-sMAX_WEBGL_VERSION=2 -s ASSERTIONS=1 -sUSE_SDL=2 --preload-file 'assets'")
else()
    target_link_libraries(COSC_Game_Jam_2023 ${SDL2_LIBRARIES})
endif()