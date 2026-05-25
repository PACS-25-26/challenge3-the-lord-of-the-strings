/**
 * @file Alias.hpp
 * @author Elena Vera Cassano and David Laurenti
 * @brief Aliases file containing all common type aliases used.
 * @version 1.0
 * @date 23-05-2026
 * @details This file mostly contains useful type aliases used in the code, 
 * together with structs that contain all the useful information for configurating
 * the problem and parallelisation.
 */

 #pragma once

 #include <iostream>
 #include <vector>
 #include <string>
 #include <cmath>
 #include <memory>
 #include <functional>
 #include <fstream>

 #include <mpi.h>
 #include <omp.h>

 #include <Eigen/Dense>
 #include <muParser.h>
 
 /** 
  * @namespace Laplace
  * @brief Namespace containing all the code related to the Laplace equation solver.
  */
 namespace Laplace{
    /** @brief Type alias for values*/
    using Real = double;

    /** @brief Type alias for x, y to manage function evaluation */
    using Coord = std::array<Real, 2>;

    /** @brief Type alias to lighten syntax */
    using Index = Eigen::Index;

    /** @brief Type alias for functions  */
    using Function = std::function<double(const Coord&)>;

    /** @brief Type alias for a vector of functions
     * @details To be used for the reading of the data file
     */
    using FunctionList = std::vector<Function>;
    /** 
     * @brief Dynamic matrix of real numbers
     * @details stored in row-major order for better performance in our use case
     */ 
    using Matrix = Eigen::Array<Real, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

    /**
    * @struct SolverConfig
    * @brief Structure containing the configuration parameters for the Jacobi solver.
    */
    struct SolverConfig{
        Index N; ///< Number of grid points in each dimension
        Real h; ///< Grid spacing
        Real tol; ///< Tolerance for convergence
        size_t max_it = 10000; ///< Maximum number of iterations
        std::string f_file; ///< File name for source term f
    };

    /**
    * @struct ParallelConfig
    * @brief Structure containing the configuration parameters for parallelization.
    */
    struct ParallelConfig{
        int rank = 0; ///< Rank of the current process
        int size = 1; ///< Total number of processes

        Index loc_rows; ///< Number of rows for the current process
        Index loc_cols; ///< Number of columns for the current process

        int rank_up; ///< Rank of process above
        int rank_down; ///< Rank of process below
    };
 }