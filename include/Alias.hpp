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

     
 }