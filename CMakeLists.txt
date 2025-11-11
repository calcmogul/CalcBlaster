cmake_minimum_required(VERSION 3.28)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(CalcBlaster)

include(FetchContent)

file(GLOB_RECURSE cpp_src src/*.cpp)
add_executable(CalcBlaster ${cpp_src})

if(NOT MSVC)
    target_compile_options(CalcBlaster PRIVATE -Wall -Wextra -pedantic)
else()
    # Suppress the following warnings:
    #   * C4244: lossy conversion
    target_compile_options(CalcBlaster PRIVATE /wd4244)
endif()
set_property(TARGET CalcBlaster PROPERTY COMPILE_WARNING_AS_ERROR ON)

target_compile_features(CalcBlaster PUBLIC cxx_std_23)
target_include_directories(CalcBlaster PRIVATE src)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)
target_link_libraries(CalcBlaster PUBLIC SFML::Audio SFML::Graphics)

FetchContent_Declare(
    box2d
    GIT_REPOSITORY https://github.com/erincatto/box2d.git
    GIT_TAG v2.4.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(box2d)
target_link_libraries(CalcBlaster PUBLIC box2d)

install(TARGETS CalcBlaster DESTINATION bin)
install(DIRECTORY resources DESTINATION bin)
