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
add_executable( solver_run
    main.cpp
    src/Solver.cpp
    src/DataManager.cpp
)

target_include_directories(solver_run PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MUPARSER_INCLUDE_DIR}
)

target_link_libraries(solver_run PRIVATE
    MPI::MPI_CXX
    OpenMP::OpenMP_CXX
    Eigen3::Eigen          
    ${MUPARSER_LIBRARY}
)

# Link runtime configuration files into the build directory.
# This keeps commands run from build/ simple without letting cfg copies go stale.
add_custom_target(cfg_links ALL
    COMMAND ${CMAKE_COMMAND} -E rm -f
        ${CMAKE_CURRENT_BINARY_DIR}/gandalf_the_white.cfg
        ${CMAKE_CURRENT_BINARY_DIR}/gandalf_the_grey.cfg
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_CURRENT_SOURCE_DIR}/gandalf_the_white.cfg
        ${CMAKE_CURRENT_BINARY_DIR}/gandalf_the_white.cfg
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_CURRENT_SOURCE_DIR}/gandalf_the_grey.cfg
        ${CMAKE_CURRENT_BINARY_DIR}/gandalf_the_grey.cfg
    COMMENT "Linking runtime configuration files into build directory"
)
add_dependencies(solver_run cfg_links)

set(GANDALF_THE_WHITE_CFG "${CMAKE_CURRENT_SOURCE_DIR}/gandalf_the_white.cfg")
set(GANDALF_THE_GREY_CFG  "${CMAKE_CURRENT_SOURCE_DIR}/gandalf_the_grey.cfg")
set(LAPLACE_MPI_PROCS 1 CACHE STRING "Number of MPI processes used by the run targets")
if(NOT MPIEXEC_EXECUTABLE)
    set(MPIEXEC_EXECUTABLE mpiexec)
endif()
if(NOT MPIEXEC_NUMPROC_FLAG)
    set(MPIEXEC_NUMPROC_FLAG -n)
endif()

function(add_laplace_run_target target_name executable_target cfg_file)
    add_custom_target(${target_name}
        COMMAND ${MPIEXEC_EXECUTABLE}
                ${MPIEXEC_NUMPROC_FLAG} ${LAPLACE_MPI_PROCS}
                $<TARGET_FILE:${executable_target}>
                ${cfg_file}
        DEPENDS ${executable_target}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running ${target_name} with ${cfg_file}"
        USES_TERMINAL
    )
endfunction()

add_laplace_run_target(run_white solver_run ${GANDALF_THE_WHITE_CFG})
add_laplace_run_target(run_grey  solver_run ${GANDALF_THE_GREY_CFG})
add_laplace_run_target(run_gandalf_the_white solver_run ${GANDALF_THE_WHITE_CFG})
add_laplace_run_target(run_gandalf_the_grey  solver_run ${GANDALF_THE_GREY_CFG})
add_laplace_run_target(gandalf_the_white solver_run ${GANDALF_THE_WHITE_CFG})
add_laplace_run_target(gandalf_the_grey  solver_run ${GANDALF_THE_GREY_CFG})

# Setup of Google Benchmark
include(FetchContent)
FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.5
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark testing" FORCE)
FetchContent_MakeAvailable(googlebenchmark)

add_executable(seqpar_test
    test/seqpar_test.cpp
    src/Solver.cpp
    src/DataManager.cpp
)

target_include_directories(seqpar_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MUPARSER_INCLUDE_DIR}
)

target_link_libraries(seqpar_test PRIVATE
    MPI::MPI_CXX
    OpenMP::OpenMP_CXX
    Eigen3::Eigen          
    ${MUPARSER_LIBRARY}
    benchmark::benchmark
)

add_executable(convergence_test
    test/convergence_test.cpp
    src/Solver.cpp
    src/DataManager.cpp
)

target_include_directories(convergence_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MUPARSER_INCLUDE_DIR}
)

target_link_libraries(convergence_test PRIVATE
    MPI::MPI_CXX
    OpenMP::OpenMP_CXX
    Eigen3::Eigen          
    ${MUPARSER_LIBRARY}
)

add_laplace_run_target(convergence_white convergence_test ${GANDALF_THE_WHITE_CFG})
add_laplace_run_target(convergence_grey  convergence_test ${GANDALF_THE_GREY_CFG})
add_laplace_run_target(convergence_gandalf_the_white convergence_test ${GANDALF_THE_WHITE_CFG})
add_laplace_run_target(convergence_gandalf_the_grey  convergence_test ${GANDALF_THE_GREY_CFG})

find_package(Doxygen QUIET)

find_program(DOC_OPEN_COMMAND NAMES xdg-open open)

if(DOXYGEN_FOUND)
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} -g Doxyfile
        COMMAND sed -i "s/^PROJECT_NAME[[:space:]]*=.*/PROJECT_NAME = \"Lord of the Strings\"/" Doxyfile
        
        # 1. Update the INPUT to include README.md
        COMMAND sed -i "s|^INPUT[[:space:]]*=.*|INPUT = ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/README.md|" Doxyfile
        
        # 2. Add the command to use README.md as the main page
        COMMAND sed -i "s|^USE_MDFILE_AS_MAINPAGE[[:space:]]*=.*|USE_MDFILE_AS_MAINPAGE = ${CMAKE_CURRENT_SOURCE_DIR}/README.md|" Doxyfile
        
        COMMAND sed -i "s/^FILE_PATTERNS[[:space:]]*=.*/FILE_PATTERNS = *.cpp *.hpp *.tpp/" Doxyfile
        COMMAND sed -i "s/^RECURSIVE[[:space:]]*=.*/RECURSIVE = YES/" Doxyfile
        COMMAND sed -i "s/^EXTRACT_ALL[[:space:]]*=.*/EXTRACT_ALL = YES/" Doxyfile
        COMMAND sed -i "s/^HAVE_DOT[[:space:]]*=.*/HAVE_DOT = YES/" Doxyfile
        COMMAND sed -i "s|^OUTPUT_DIRECTORY[[:space:]]*=.*|OUTPUT_DIRECTORY = ${CMAKE_CURRENT_BINARY_DIR}/doc|" Doxyfile
        COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating documentation ..."
        VERBATIM
    )
    if(DOC_OPEN_COMMAND)
        add_custom_target(doc_open
            COMMAND ${DOC_OPEN_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}/doc/html/index.html
            DEPENDS doc
            COMMENT "Opening HTML documentation..."
            VERBATIM
        )
    endif()
else()
    message(WARNING "Doxygen not found on system. Documentation target will not be available.")
endif()
