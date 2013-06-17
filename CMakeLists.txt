cmake_minimum_required(VERSION 2.4)

project(sqlite3)
set(GEOS_ENABLE_TESTS false)
set(CMAKE_BUILD_TYPE Release)
add_subdirectory(mitab-1.7.0 mitab)
add_subdirectory(geos-3.3.2 geos)
add_subdirectory(libiconv-1.11.1 iconv)
add_subdirectory(sqlite3-3.7.17 sqlite3)
