/**
 * @file Alias.hpp
 * @author Elena Vera Cassano and David Laurenti
 * @brief Aliases file containing all common type aliases used.
 * @version 1.0
 * @date 23-05-2026
 * @details 
 */

 #pragma once

 #include <iostream>
 #include <vector>
 #include <string>
 #include <cmath>
 #include <memory>


 #include <mpi.h>
 #include <omp.h>

 #include <Eigen/Dense>
 #include <muParser.h>
 
 /** 
  * @namespace Laplace
  * @brief Namespace containing all the code related to the Laplace equation solver.
  */
 namespace Laplace{
    // Type alias for real numbers
    using Real = double;

    // Type alias for indices
    using Index = Eigen::Index;

    // Dynamic matrix of real numbers, stored in row-major order for better performance in our use case
    using Matrix = Eigen::Array<Real, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

    /*
    * @struct SolverConfig
    * @brief Structure containing the configuration parameters for the Jacobi solver.
    */
    struct SolverConfig{
        Index N; // Number of grid points in each dimension
        Real h; // Grid spacing
        Real tol; // Tolerance for convergence
        size_t max_it = 10000; // Maximum number of iterations
        std::string f_file; // File name for source term f
    };

    /*
    * @struct ParallelConfig
     *@brief Structure containing the configuration parameters for parallelization.
    */
    struct ParallelConfig{
        int rank = 0; // Rank of the current process
        int size = 1; // Total number of processes

        Index loc_rows; // Number of rows for the current process
        Index loc_cols; // Number of columns for the current process

        int rank_up; // Rank of process above
        int rank_down; // Rank of process below
    };
 }