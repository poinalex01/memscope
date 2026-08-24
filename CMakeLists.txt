cmake_minimum_required(VERSION 3.20)
project(MemScope LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.11.2/msvc2022_64")

add_subdirectory(src/engine)
add_subdirectory(src/cli)
add_subdirectory(src/gui)