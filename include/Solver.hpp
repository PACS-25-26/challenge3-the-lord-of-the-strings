/**
 * @file Solver.hpp
 * @author Elena Vera Cassano and David Laurenti
 * @brief Header file for Solver class, which implements Jacobi algorithm to solve the Laplace equation.
 * @version 1.0
 * @date 24-05-2026
 * @details
 */

 #pragma once
 #include "Alias.hpp"

 /**
  * @namespace Laplace
  * @brief Namespace containing the implementation of the Jacobi algorithm to solve the Laplace equation
  */
 namespace Laplace{
    /**
    * @class Solver
    * @brief Class implementing the Jacobi algorithm to solve the Laplace equation.
    * @details The solver is the most important feature of the project. 
    * @invariant
    * @note 
    */
    class Solver {
        private:
            Matrix U;
            Matrix F; // Capire se metterla solo in solve()

            SolverConfig s_config; // Capire se fare struct o dati singoli
            ParallelConfig p_config;

            bool check_convergence(const Matrix& U_old, const Matrix& U_new);

        public:
            Solver(const SolverConfig& s, const ParallelConfig& p);

            void solve();

            void convert_to_vtk(const std::string& file_name);
    };
 }