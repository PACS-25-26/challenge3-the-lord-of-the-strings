/**
 * @file Utils.hpp
 * @author Elena Vera Cassano and David Laurenti
 * @brief Header file for utility functions used in the project.
 * @version 1.0
 * @date 24-05-2026
 * @details This file provides all needed functions to handle file reading, 
 * calling the solver, and calling parallel features that the code provides.
 */

 #pragma once

 #include "Alias.hpp"

 /**
  * @namespace Laplace
  * @brief Namespace containing all the code related to the Laplace equation solver.
  */
 namespace Laplace{
   
   /**
    * @brief Function that computes value of force * \f$ f(x_i,\ y_i) \f$
    * 
    * @param file_name file to be read
    * @return A matrix containing the function evaluated over the grid
    */
    Laplace::Matrix compute_F(const std::string& file_name);

    void init_parallel(Laplace::ParallelConfig& p_config, Laplace::Index N);

    void init_solver(Laplace::SolverConfig& s_config, const std::string& config_file);

   }