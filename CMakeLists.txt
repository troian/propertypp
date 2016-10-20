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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package(SQLite3 REQUIRED)

check_include_file_cxx(iostream HAVE_IOSTREAM)
check_include_file_cxx(string HAVE_MEMORY)
check_include_file_cxx(exception HAVE_EXCEPTION)
check_include_file_cxx(stdexcept HAVE_STDEXCEPT)
check_include_file(sqlite3.h HAVE_SQLITE3)

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
