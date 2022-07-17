cmake_minimum_required(VERSION 3.12)

if(CMAKE_POLICY_DEFAULT_CMP0017 OR CMAKE_POLICY_DEFAULT_CMP0020)
	# touch these to remove warnings
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

project(cgci VERSION 0.0.0 LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_compile_definitions(NAME="${CMAKE_PROJECT_NAME}")
add_compile_definitions(VERSION="${CMAKE_PROJECT_VERSION}")
add_compile_definitions(CACHE_DIR="/var/cache/${CMAKE_PROJECT_NAME}")

message(STATUS "Name: ${CMAKE_PROJECT_NAME}")
message(STATUS "Version: ${CMAKE_PROJECT_VERSION}")

add_subdirectory(src)