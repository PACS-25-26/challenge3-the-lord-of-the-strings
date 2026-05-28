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
 #include "DataManager.hpp"

 /**
  * @namespace Laplace
  * @brief Namespace containing the implementation of the Jacobi algorithm to solve the Laplace equation
  */
 namespace Laplace{
    /**
    * @class Solver
    * @brief Class implementing the Jacobi algorithm to solve the Laplace equation.
    * @details The solver is the most important feature of the project. 
    */
    class Solver {
        private:
            /**
             * @brief Matrix representing local processor solution at each iteration.
             * @details The matrix U is updated iteratively in the solve() method. After convergence, it contains the final solution of the Laplace equation for the local portion of the domain handled by the processor.
             */
            Matrix U;

            /**
             * @brief Matrix representing the source term of the Laplace equation.
             * @details The matrix F is initialized based on the source function provided in the SolverConfig. It is used in the iterative update of the solution matrix U in the solve() method.
             */
            Matrix F;

            /**
             * @brief Configuration parameters for the solver of the Laplace equation.
             * @details This struct contains all the necessary parameters to define the problem to be solved, such as the exact solution, the source term, the Dirichlet boundary conditions, the number of grid points, the grid spacing, the tolerance for convergence, and the maximum number of iterations.
             */
            SolverConfig s_config;

            /**
             * @brief Configuration parameters for parallel execution.
             * @details This struct contains all the necessary parameters for parallelization, such as rank, size, local number of rows and columns, ranks of neighboring processes.
             */
            ParallelConfig p_config;
            
            /**
             * @brief Initializes the solver based on the provided configurations.
             * @details This function sets up initial conditions, and is called by both constructors to ensure that the solver is properly initialized regardless of how it is constructed.
             */ 
            void setup();

            /**
             * @brief Checks for convergence of the Jacobi method.
             * @param U_old The solution matrix from the previous iteration.
             * @param U_new The solution matrix from the current iteration.
             * @return True if the method has converged, false otherwise.
             * @details The convergence is checked by computing the mean squared error between U_old and U_new.
             */
            bool check_convergence(const Matrix& U_old, const Matrix& U_new);
            
            /**
             * @brief Applies the Dirichlet boundary conditions to the solution matrix U.
             * @details This function modifies the matrix U in-place to enforce the Dirichlet boundary conditions.
             */
            void apply_dirichlet_conditions();

        public:
            /** 
             * @brief Constructor for the Solver class.
             * @param s Configuration for the solver.
             * @param p Configuration for parallel execution.
             * @details The constructor initializes the matrix U and source F based on given configurations.
             */
            Solver(const SolverConfig& s, const ParallelConfig& p);
            
            /**
             * @brief Constructor for the Solver class.
             * @param config_file The name of the configuration file containing the solver and parallel parameters.
             * @details This constructor reads the configuration from a file and initializes the solver accordingly.
             */
            Solver(const std::string& config_file);

            /**
             * @brief Solves the Laplace equation using the Jacobi method.
             * @details The method firstly applies the Dirichlet boundary conditions, then iteratively updates the solution matrix U until convergence or until the maximum iterations are reached.
             */
            void solve();

            /**
             * @brief Gathers local solutions from ranks, puts them together and writes the final matrix U into a VTK format file.
             * @param file_name The name of the output VTK file.
             * @details This function gathers the solution from all processes to the master process, which computes a global solution matrix
             * and writes it to a VTK file for visualization.
             */
            void convert_to_vtk(const std::string& file_name);
    };
 }