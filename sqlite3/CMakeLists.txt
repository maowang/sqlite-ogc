cmake_minimum_required(VERSION 2.4)

file(GLOB_RECURSE SQLITE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB_RECURSE SQLITE_SOURCE_CPP ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE SQLITE_EXTEND_SOURCE ${PROJECT_SOURCE_DIR}/extend/*.c)

include_directories(${PROJECT_BINARY_DIR}/geos/include)
include_directories(${PROJECT_BINARY_DIR}/libiconv/include)
include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

add_definitions(-DLIBICONV_PLUG)
add_definitions(-D_REENTRANT=1 -DSQLITE_THREADSAFE=1 -DSQLITE_ENABLE_FTS3 -DSQLITE_ENABLE_RTREE)

link_directories(${PROJECT_BINARY_DIR}/libs)

add_executable(sqlite3 ${SQLITE_SOURCE} ${SQLITE_EXTEND_SOURCE} ${SQLITE_SOURCE_CPP})

target_link_libraries(sqlite3 iconv)
target_link_libraries(sqlite3 geos-static)
target_link_libraries(sqlite3 geos_c)

if(NOT MSVC)
target_link_libraries(sqlite3 pthread)
target_link_libraries(sqlite3 dl)
endif()

if(CYGWIN)
	target_link_libraries(sqlite3 cygwin)
endif()

