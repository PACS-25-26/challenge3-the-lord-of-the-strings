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
    
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::vector<std::string> data_vec(9); // Assuming we have 9 lines of configuration data to read

    if(rank == 0)
    {
        //check for correct input file
        if (argc < 2) {
            std::cerr << "Not enough arguments!" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        std::string file_name = argv[1];

        // master process reads the data from the file and stores it in a vector of strings
        data_vec = Laplace::read_data(file_name);   
    }

    // Broadcast the vector of strings to all processes
    for(int i = 0; i < data_vec.size(); ++i) {
        int str_length = 0;
        if(rank == 0)
        {
            str_length = data_vec[i].size();
        }
        // ommunicate the length of the string first to prepare buffers on other processes
        MPI_Bcast(&str_length, 1, MPI_INT, 0 , MPI_COMM_WORLD);
        if(rank != 0)
        {
            data_vec[i].resize(str_length);
        }
        // communicate vector
        MPI_Bcast(data_vec[i].data(), str_length, MPI_CHAR, 0 , MPI_COMM_WORLD);
    }

    // Now each process can independently process the data and create its own struct of solver configurations
    
    Laplace::SolverConfig s_config = Laplace::process_data(data_vec);
    Laplace::ParallelConfig p_config = Laplace::process_parallel_config(s_config.N);

    Laplace::Solver solver(s_config, p_config);
    solver.solve();
    solver.print_number_of_iterations();
    solver.convert_to_vtk("solution.vtk");
    
    if(rank == 0)
    {
    // for testing, we can evaluate the exact solution and the forcing term at some points to check if they are correct.
    Laplace::Function exact_sol = s_config.u_ex;
    Laplace::Function f_forcing = s_config.f;
    
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
    }

    MPI_Finalize();
    return 0;
}
