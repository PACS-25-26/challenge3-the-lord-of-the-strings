/**
 * @file main.cpp
 * @author Elena Vera Cassano and David Laurenti
 * @brief Main file for the Laplace equation solver project.
 * @version 1.0
 * @date 24-05-2026
 * @details
 */

#include "include/Alias.hpp"
#include "include/Solver.hpp"
#include "include/DataManager.hpp"

int main(int argc, char** argv) {
    
    //check for correct input file
    if (argc < 3) {
        std::cerr << "Not enough arguments!" << std::endl;
        return 1; 
    }

    std::string file_name = argv[1];
    int processes = std::stoi(argv[2]);

    Laplace::SolverConfig s_config = read_data(file_name);
    auto exact_sol = s_config.u_ex;
    auto f_forcing = s_config.f;

    Laplace:ParallelConfig p_config;

    Laplace::Solver solver(s_config, p_config);
    
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    Laplace::SolverConfig s_config;
    Laplace::init_solver(s_config, file_name);

    Laplace::ParallelConfig p_config;
    Laplace::init_parallel(p_config, s_config.N);

    Laplace::Solver solver(s_config, p_config);
    solver.solve();
    if (p_config.rank == 0) {
        solver.convert_to_vtk("solution.vtk");
    }
    
    MPI_Finalize();
    
    Laplace::Coord coords = {0.0, 0.0};

    Laplace::Coord punto1 = {0.25, 0.25};
    Laplace::Coord punto2 = {0.5, 0.5};

    std::cout << "\n--- Test Punto 1 (x=0.25, y=0.25) ---" << std::endl;
    std::cout << "Soluzione Esatta: " << exact_sol(punto1) << " (Dovrebbe essere 1)" << std::endl;
    std::cout << "Forzante F:       " << f_forcing(punto1) << " (Dovrebbe essere ~78)" << std::endl;

    std::cout << "\n--- Test Punto 2 (x=0.5, y=0.5) ---" << std::endl;
    std::cout << "Soluzione Esatta: " << exact_sol(punto2) << " (Dovrebbe essere 0.0)" << std::endl;
    std::cout << "Forzante F:       " << f_forcing(punto2) << " (Dovrebbe essere ~0.0)" << std::endl;
    
    std::cout << "\n================================================" << std::endl;

    return 0;
}