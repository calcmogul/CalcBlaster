cmake_minimum_required(VERSION 3.28)

# Set default build type to release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

# Generate compile_commands.json by default
if(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_EXPORT_COMPILE_COMMANDS "YES" CACHE STRING "" FORCE)
endif()

project(CalcBlaster)

file(GLOB_RECURSE cpp_src src/*.cpp)
add_executable(CalcBlaster ${cpp_src})

if(NOT MSVC)
    target_compile_options(
        CalcBlaster
        PRIVATE -Wall -pedantic -Wextra -Werror
    )
else()
    # Suppress the following warnings:
    #   * C4244: lossy conversion
    target_compile_options(CalcBlaster PRIVATE /wd4244 /WX)
endif()

target_compile_features(CalcBlaster PUBLIC cxx_std_23)
target_include_directories(CalcBlaster PRIVATE src)

include(FetchContent)

fetchcontent_declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
fetchcontent_makeavailable(SFML)
target_link_libraries(CalcBlaster PUBLIC SFML::Audio SFML::Graphics)

fetchcontent_declare(
    box2d
    GIT_REPOSITORY https://github.com/erincatto/box2d.git
    GIT_TAG v2.4.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
fetchcontent_makeavailable(box2d)
target_link_libraries(CalcBlaster PUBLIC box2d)

install(TARGETS CalcBlaster DESTINATION bin)
install(DIRECTORY resources DESTINATION bin)
