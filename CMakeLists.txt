cmake_minimum_required(VERSION 2.4)

project(sqlite3)
set(GEOS_ENABLE_TESTS false)
set(CMAKE_BUILD_TYPE Debug)
add_subdirectory(geos-3.3.2 geos)
add_subdirectory(libiconv-1.11.1 libiconv)
add_subdirectory(sqlite3 bin)
