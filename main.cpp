/**
 * @file main.cpp
 * @author Elena Vera Cassano and David Laurenti
 * @brief Main file for the Laplace equation solver project.
 * @version 1.0
 * @date 24-05-2026
 * @details
 */

#include "include/Utils.hpp"

int main(int argc, char** argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    Laplace::SolverConfig s_config;
    Laplace::init_solver(s_config, "config.txt");

    Laplace::ParallelConfig p_config;
    Laplace::init_parallel(p_config, s_config.N);

    Laplace::Solver solver(s_config, p_config);
    solver.solve();
    if (p_config.rank == 0) {
        solver.convert_to_vtk("solution.vtk");
    }
    
    MPI_Finalize();
    return 0;
}
