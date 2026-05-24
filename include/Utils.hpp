/**
 * @file Utils.hpp
 * @author Elena Vera Cassano and David Laurenti
 * @brief Header file for utility functions used in the project.
 * @version 1.0
 * @date 24-05-2026
 * @details
 */

 #pragma once

 #include "Alias.hpp"
 #include "Solver.hpp"

 /**
  * @namespace Laplace
  * @brief Namespace containing all the code related to the Laplace equation solver.
  */
 namespace Laplace{
    Laplace::Matrix compute_F(const std::string& file_name);

    void init_parallel(Laplace::ParallelConfig& p_config, Laplace::Index N);

    void init_solver(Laplace::SolverConfig& s_config, const std::string& config_file);
 }