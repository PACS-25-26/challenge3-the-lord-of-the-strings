cmake_minimum_required(VERSION 3.15)
project(LaplaceSolver CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Looking for packages
find_package(MPI REQUIRED)

find_package(OpenMP REQUIRED)

find_package(Eigen3 QUIET NO_MODULE)
if(NOT Eigen3_FOUND)
    find_path(EIGEN3_INCLUDE_DIR NAMES Eigen/Dense PATH_SUFFIXES eigen3)
    if(NOT EIGEN3_INCLUDE_DIR)
        message(FATAL_ERROR "Eigen3 not found.")
    endif()
    add_library(Eigen3::Eigen INTERFACE IMPORTED)
    target_include_directories(Eigen3::Eigen INTERFACE ${EIGEN3_INCLUDE_DIR})
endif()

find_path(MUPARSER_INCLUDE_DIR NAMES muParser.h PATH_SUFFIXES muparser)
find_library(MUPARSER_LIBRARY NAMES muparser)
if(NOT MUPARSER_INCLUDE_DIR OR NOT MUPARSER_LIBRARY)
    message(FATAL_ERROR "muParser not found.")
endif()

# Executable
add_executable(test_parser
    main.cpp
    src/Utils.cpp
    src/Solver.cpp
)

target_include_directories(test_parser PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MUPARSER_INCLUDE_DIR}
)

target_link_libraries(test_parser PRIVATE
    MPI::MPI_CXX
    OpenMP::OpenMP_CXX
    Eigen3::Eigen          
    ${MUPARSER_LIBRARY}
)

# Copy txt file into build directory
add_custom_command(
    TARGET test_parser POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/data.txt
        ${CMAKE_CURRENT_BINARY_DIR}/data.txt
    COMMENT "Syncing data.txt to build directory"
)