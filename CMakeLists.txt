cmake_minimum_required(VERSION 3.2)

project(propertypp LANGUAGES CXX)

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

option(PROPERTY_SQLITE "Build SQLite storage implementation" ON)
option(PROPERTY_BUILD_SHARED_LIB "Build the shared library" OFF)
option(PROPERTY_TESTS "Build tests" OFF)

check_include_file_cxx(iostream HAVE_IOSTREAM)
check_include_file_cxx(string HAVE_STRING)
check_include_file_cxx(exception HAVE_EXCEPTION)
check_include_file_cxx(stdexcept HAVE_STDEXCEPT)
check_include_file_cxx(memory HAVE_MEMORY)

set(EXPORT_INCLUDES
	${CMAKE_CURRENT_SOURCE_DIR}/include/export
)

set(LIB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/property.cpp
    ${EXPORT_INCLUDES}/base/propertypp/property.hpp
)

#if (NOT PROPERTY_SQLITE)
#	message(FATAL_ERROR "noone from any of available property backends configured")
#endif()

if (PROPERTY_SQLITE)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(SQLITE3 REQUIRED sqlite3)
	check_include_file(sqlite3.h HAVE_MEMORY)

	set(LIB_SOURCES ${LIB_SOURCES}
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/sqlite.cpp
	    ${EXPORT_INCLUDES}/sqlite/propertypp/sqlite.hpp
	)
endif()

add_library(
	${PROJECT_NAME} STATIC ${LIB_SOURCES}
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	${EXPORT_INCLUDES}/base
)

target_link_libraries(
	${PROJECT_NAME}
	base64
)

if (PROPERTY_SQLITE)
	target_include_directories(
		${PROJECT_NAME}
		PUBLIC
		${SQLITE3_INCLUDE_DIRS}
		${EXPORT_INCLUDES}/sqlite
	)

	target_link_libraries(
		${PROJECT_NAME}
		${SQLITE3_LIBRARIES}
	)
endif()

if (PROPERTY_TESTS)
	enable_testing()
	add_subdirectory(gtest)

	include_directories(${PROJECT_SOURCE_DIR}/gtest/googletest/include)

	if (PROPERTY_SQLITE)
		set(PROPERTY_SQLITE_TEST_SRS ${CMAKE_CURRENT_SOURCE_DIR}/tests/sqlite_test.cpp)

		add_executable(property_sqlite_test ${PROPERTY_SQLITE_TEST_SRS})

		target_link_libraries(
			property_sqlite_test

			propertypp
			gtest_main
		)

		add_test(UnitTests property_sqlite_test)
	endif()
endif()
