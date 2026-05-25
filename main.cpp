/**
 * @file main.cpp
 * @author Elena Vera Cassano and David Laurenti
 * @brief Main file for the Laplace equation solver project.
 * @version 1.0
 * @date 24-05-2026
 * @details
 */

#include "include/Utils.hpp"
#include "include/Alias.hpp"
int main(int argc, char** argv) {
    
    /* int provided;
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
    */

    Laplace::Coord coords = {0.0, 0.0};

    auto test_funcs = Laplace::read_data("data.txt" , &coords);
    auto exact_sol = test_funcs[0];
    auto f_forcing = test_funcs[1];

    Laplace::Coord punto1 = {0.25, 0.25};
    Laplace::Coord punto2 = {0.5, 0.5};

    std::cout << "\n--- Test Punto 1 (x=0.25, y=0.25) ---" << std::endl;
    std::cout << "Soluzione Esatta: " << exact_sol(punto1) << " (Dovrebbe essere 0.5)" << std::endl;
    std::cout << "Forzante F:       " << f_forcing(punto1) << " (Dovrebbe essere ~39.47)" << std::endl;

    std::cout << "\n--- Test Punto 2 (x=0.5, y=0.5) ---" << std::endl;
    std::cout << "Soluzione Esatta: " << exact_sol(punto2) << " (Dovrebbe essere 1.0)" << std::endl;
    std::cout << "Forzante F:       " << f_forcing(punto2) << " (Dovrebbe essere ~78.95)" << std::endl;
    
    std::cout << "\n================================================" << std::endl;

    return 0;
}
