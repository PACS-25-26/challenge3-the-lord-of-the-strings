#include "../include/Solver.hpp"
#include "../include/DataManager.hpp"
#include "../include/Alias.hpp"

int main(int argc, char** argv){
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

    
    std::vector<Laplace::Real> errors(4);

    try {
        Laplace::SolverConfig s_config = Laplace::process_data(data_vec);

        if(rank == 0){
            std::cout << "N\tError\n";
        }

        for(Laplace::Index k = 4; k < 8; ++k){
            s_config.N = std::pow(2, k);
            
            Laplace::ParallelConfig p_config = Laplace::process_parallel_config(s_config.N);
            Laplace::Solver solver(s_config, p_config);
            
            solver.solve();
            
            errors[k-4] = solver.compute_error();

            if(rank == 0){
                std::cout << s_config.N << "\t" << errors[k-4] << "\n";
            }
        }
    }
    catch (mu::Parser::exception_type &e) {
        std::cerr << "[Rank " << rank << "] muParser Error: " << e.GetMsg() 
                  << " (nell'espressione: " << e.GetExpr() << ")" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    catch (std::exception &e) {
        std::cerr << "[Rank " << rank << "] C++ Exception: " << e.what() << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Finalize();
    return 0;
}
