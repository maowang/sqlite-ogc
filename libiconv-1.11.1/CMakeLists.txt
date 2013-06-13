file(GLOB_RECURSE ICONV_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.c)
file(GLOB_RECURSE ICONV_SOURCE_CHARSET ${CMAKE_CURRENT_SOURCE_DIR}/libcharset/*.c)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/libs)

if(CMAKE_COMPILER_IS_GNUGCC OR CMAKE_COMPILER_IS_GNUGXX)
	set(HAVE_VISIBILITY 1)
else()
	set(HAVE_VISIBILITY 0)
endif()

set(USE_MBSTATE_T 0)
set(BROKEN_WCHAR_H 0)
set(HAVE_WCHAR_T 1)
set(ICONV_CONST "const")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libcharset/include/localcharset.h.build.in ${CMAKE_CURRENT_BINARY_DIR}/include/localcharset.h @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/lib/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/config.h @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/iconv.h.build.in ${CMAKE_CURRENT_BINARY_DIR}/include/iconv.h @ONLY)

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/include/config.h "\#define HAVE_STRING_H 1\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/include/config.h "\#define HAVE_WCHAR_T 1\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/include/config.h "\#define HAVE_BTOWC 1\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/include/config.h "\#define HAVE_MBSINIT 1\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/include/config.h "\#define HAVE_MBRTOWC 1\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/include/config.h "\#define HAVE_MBRLEN 1\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/include/config.h "\#define HAVE_MBSRTOWCS 1\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/include/config.h "\#define HAVE_MBSNRTOWCS 1\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/include/config.h "\#define HAVE_WCRTOMB 1\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/include/config.h "\#define HAVE_WCSRTOMBS 1\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/include/config.h "\#define HAVE_WCSNRTOMBS 1\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/include/config.h "\#define HAVE_DECL_WCTOB 1\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/include/config.h "\#define HAVE_DECL_WCWIDTH 1\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/include/config.h "\#define LIBDIR \"${CMAKE_BINARY_DIR}/libs\"\n")

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

add_definitions(-DHAVE_CONFIG_H)
add_definitions(-DLIBICONV_PLUG)
add_definitions(-DBUILDING_LIBICONV)
add_library(iconv STATIC ${ICONV_SOURCE} ${ICONV_SOURCE_CHARSET})

