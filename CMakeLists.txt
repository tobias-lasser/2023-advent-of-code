cmake_minimum_required(VERSION 3.26)
project(2023_advent_of_code LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Eigen unfortunately required for AoC24
include(FetchContent)
FetchContent_Declare(
        Eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG master
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(EIGEN_BUILD_DOC OFF)
set(EIGEN_BUILD_PKGCONFIG OFF)
FetchContent_MakeAvailable(Eigen)


add_executable(AoC1 01.cpp)
add_executable(AoC2 02.cpp)
add_executable(AoC3 03.cpp)
add_executable(AoC4 04.cpp)
add_executable(AoC5 05.cpp)
add_executable(AoC6 06.cpp)
add_executable(AoC7 07.cpp)
add_executable(AoC8 08.cpp)
add_executable(AoC9 09.cpp)
add_executable(AoC10 10.cpp)
add_executable(AoC11 11.cpp)
add_executable(AoC12 12.cpp)
add_executable(AoC13 13.cpp)
add_executable(AoC14 14.cpp)
add_executable(AoC15 15.cpp)
add_executable(AoC16 16.cpp)
add_executable(AoC17 17.cpp)
add_executable(AoC18 18.cpp)
add_executable(AoC19 19.cpp)
add_executable(AoC20 20.cpp)
add_executable(AoC21 21.cpp)
add_executable(AoC22 22.cpp)
add_executable(AoC23 23.cpp)
add_executable(AoC24 24.cpp)
target_link_libraries(AoC24 PRIVATE Eigen3::Eigen)
add_executable(AoC25 25.cpp)
