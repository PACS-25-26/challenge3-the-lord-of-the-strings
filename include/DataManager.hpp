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
 *  -# Given force
 *  -# Dirichlet boundary conditions given in the following order:\n
 *   \f$ x = 0;\ x = 1;\ y = 0;\ y = 1. \f$
 * - Solver parameters
 *  -# The number of grid points in each dimension
 *  -# Grid spacing
 *  -# Tolerance for convergence criteria
 *  -# Maximum number of iterations
 * While the latter contains all information about the parallel part.
 * 
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
    Function u_ex;
    Function f;
    Function cond1;
    Function cond2;
    Function cond3;
    Function cond4;
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
     * @brief Reader of data.txt file that parses all given functions

     * The functions are all read and parsed into functions, and added into a vector
     * that will be provided to the solver.
     * @param filename file to be read
     * @param coord Coordinates to be evaluated
     * @return std::vector containing all parsed functions in described order
     */
    FunctionList read_data (const std::string& filename, Coord* coord);
} 

