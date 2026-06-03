c/**
 * @file DataManager.hpp
 * @author Elena Cassano and David Laurenti
 * @brief This file contains all useful structures and functions to manage
 * data regarding the problem
 * @version 0.1
 * @date 2026-05-25
 * 
 * @details The information is divided between two structs , SolverConfig and ParallelConfig.
 * The former contains all data regarding the problem to be solved, that is:
 * - Dirichlet problem
 *  -# Exact solution
 *  -# Given source
 *  -# Dirichlet boundary conditions given in the following order:\n
 *   \f$ y = 1;\ x = 1;\ y = 0;\ x = 0. \f$
 * - Solver parameters
 *  -# The number of grid points in each dimension
 *  -# Grid spacing
 *  -# Tolerance for convergence criteria
 *  -# Maximum number of iterations
 * While the latter contains all information about the parallel part.
 * The informations are read from a provided "text_case.cfg" configuration file through a provided function. 
 */

#pragma once

#include "Alias.hpp"

namespace Laplace
{
    /**
    * @struct SolverConfig
    * @brief Structure containing the configuration parameters for the Jacobi solver.
    */
    struct SolverConfig{
        Function u_ex; ///< exact solution
        Function f; ///< source
        Function cond1; ///< Dirichlet condition y=1
        Function cond2; ///< Dirichlet condition x=1
        Function cond3; ///< Dirichlet condition y=0
        Function cond4; ///< Dirichlet condition x=0
        Index N; ///< Number of grid points in each dimension
        Real h; ///< Grid spacing
        Real tol; ///< Tolerance for convergence
        size_t max_it = 10000; ///< Maximum number of iterations
    };

    /**
    * @struct ParallelConfig
    *@brief Structure containing the configuration parameters for parallelization.
    */
    struct ParallelConfig{
        int rank = 0; ///< Rank of the current process
        int size = 1; ///< Total number of processes

        Index start_row; ///< Starting row index for the current process
        Index loc_rows; ///< Number of rows for the current process
        Index loc_cols; ///< Number of columns for the current process

        int rank_up; ///< Rank of process above, towards y=1
        int rank_down; ///< Rank of process below, towards y=0
    };

    /**
     * @brief External function to safely parse the expressions provided
     * 
     * @param expr function expression
     * @return Function ready for evaluation
     * @details This function uses a class implemented in the cpp (as that is its only use)
     * that provides a function parser that is safe for shared memory applications, making use of
     * unique pointers, and preventing copy constructions that can be dangerous in cases of shared memory.
     */
    Function make_parsed_function(const std::string& expr);

    /**
     * @brief Reader of test_case.cfg file.
     * 
     * @details The function reads all data and based on the scope and provides a vector
     * of strings with all the lines read. This is useful to separate from the parsing and elaboration
     * of the data, to facilitate the MPI communication. In particular, this function is to be called
     * only by the master process, that will then easily broadcast the vector of strings to each process,
     * that will autonomously process the data and store it in the struct to avoid problems with shared memory and pointers.
     * In this way we avoid the need of complex MPI communication for the functions.
     * @param filename file to be read containing all information
     * @return vector of string containing all the lines read from the file
     */
    std::vector<std::string> read_data(const std::string& filename);

    /**
     * @brief Function that creates struct of solver configuration from provided vector of strings.
     * @details The function reads all data provided in a vector of strings and parses the functions
     * or converts numerical data. In particular it makes use of make_parsed_function for 
     * the provided function expressions, and puts the data in a struct ready to be provided to the solver.
     * @param s_vec vector of string containing all the lines read from the file, to be processed and stored in the struct
     * @return data striuct with all the data properly stored and parsed.
     */
    SolverConfig process_data(const std::vector<std::string>& s_vec);

    /**
     * @brief Function that creates struct of parallel configuration from the global grid size.
     * @details The function calculates the local grid dimensions and neighboring process ranks based on the global grid size and MPI communicator data.
     * @param N The global grid size
     * @return data struct with all the parallel configuration parameters properly stored.
     */
    ParallelConfig process_parallel_config(Index N);
}
