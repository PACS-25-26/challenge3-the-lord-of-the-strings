/**
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
 *   \f$ x = 0;\ x = 1;\ y = 0;\ y = 1. \f$
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
    Function cond1; ///< Dirichlet condition x=0 
    Function cond2; ///< Dirichlet condition x=1
    Function cond3; ///< Dirichlet condition y=0
    Function cond4; ///< Dirichlet condition y=1
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

    Index loc_rows; ///< Number of rows for the current process
    Index loc_cols; ///< Number of columns for the current process

    int rank_up; ///< Rank of process above
    int rank_down; ///< Rank of process below
    };

    /**
     * @brief Reader of test_case.cfg file that parses all given functions
     * 
     * @details The function reads all data and based on the scope , parses the functions
     * or directly stores the information.
     * @param filename file to be read containing 
     * @return std::vector containing all parsed functions in described order
     */
    SolverConfig read_data(const std::string& filename);
} 

