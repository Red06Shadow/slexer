cmake_minimum_required(VERSION 3.15)
project(slexer 
        VERSION 0.1.0 
        LANGUAGES CXX
)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/release)

add_library(slexer INTERFACE)

target_include_directories(slexer 
    INTERFACE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

target_compile_definitions(slexer INTERFACE MYREGEX_MAJOR_VERSION=1 MYREGEX_MINOR_VERSION=0)

install(DIRECTORY include/
DESTINATION include
FILES_MATCHING PATTERN "*.hpp")


option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
        enable_testing()
        add_subdirectory(test)
endif()