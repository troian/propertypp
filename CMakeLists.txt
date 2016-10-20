cmake_minimum_required(VERSION 3.2)

project(propertypp LANGUAGES C CXX)

include(CheckCSourceCompiles)
include(CheckCCompilerFlag)
include(CheckCXXSourceCompiles)
include(CheckCXXSymbolExists)
include(CheckCXXCompilerFlag)
include(CheckFunctionExists)
include(CheckTypeSize)
include(CheckIncludeFiles)
include(CheckIncludeFileCXX)

check_include_file_cxx(iostream HAVE_IOSTREAM)
check_include_file_cxx(memory HAVE_MEMORY)
check_include_file_cxx(functional HAVE_FUNCTIONAL)
check_include_file_cxx(string HAVE_STRING)
check_include_file_cxx(fstream HAVE_FSTREAM)
check_include_file_cxx(iostream HAVE_FSTREAM)
check_include_file_cxx(thread HAVE_THREAD)
check_include_file_cxx(atomic HAVE_ATOMIC)
check_include_file_cxx(array HAVE_ARRAY)
check_include_file_cxx(map HAVE_MAP)
check_include_file_cxx(list HAVE_LIST)
check_include_file_cxx(queue HAVE_QUEUE)
check_include_file_cxx(vector HAVE_VECTOR)
check_include_file_cxx(algorithm HAVE_ALGORITHM)
check_include_file_cxx(exception HAVE_EXCEPTION)
check_include_file_cxx(stdexcept HAVE_STDEXCEPT)
check_include_file_cxx(mutex HAVE_MUTEX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package(SQLite3 REQUIRED)

message("SQLITE3_LIBRARIES ${SQLITE3_LIBRARIES}")
#if (NOT Boost_FOUND)
#	message(FATAL_ERROR "Boost required")
#endif ()

#if (NOT LIBSSH_LIBRARIES)
#	message(FATAL_ERROR "LibSSH required")
#endif()

#if (NOT OPENSSL_FOUND)
#	message(FATAL_ERROR "OpenSSL required")
#endif()

#include_directories(${OPENSSL_INCLUDE_DIR})
#include_directories(${LIBSSH_INCLUDE_DIRS})
#include_directories(${Boost_INCLUDES})
#
#link_directories(${Boost_LIBRARY_DIRS})

set(EXPORT_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/export
)

add_library(
	${PROJECT_NAME} STATIC

	${CMAKE_CURRENT_SOURCE_DIR}/src/property.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/sqlite.cpp

	${EXPORT_INCLUDES}/propertypp/property.hpp
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	${EXPORT_INCLUDES}
)

target_link_libraries(
	${PROJECT_NAME}
    sqlite3
)
