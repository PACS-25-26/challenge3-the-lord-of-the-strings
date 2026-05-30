#include "Solver.hpp"

namespace Laplace{
    bool Solver::check_convergence(const Matrix& U_old, const Matrix& U_new){
        Index rows = p_config.loc_rows;
        Index cols = p_config.loc_cols;
        Real h = s_config.h;

        Real local_mse = 0.0;

        #pragma omp parallel for collapse(2) reduction(+:local_mse)
        for(Index i = 0; i < rows; ++i){
            for(Index j = 0; j < cols; ++j){
                Real diff = U_new(i, j) - U_old(i, j);
                local_mse += diff * diff;
            }
        }

        local_mse = std::sqrt(h*local_mse);

        int local_convergence = (local_mse < s_config.tol) ? 1 : 0;

        int global_convergence(0);

        MPI_Allreduce(&local_convergence, &global_convergence, 1, MPI_INT, MPI_LAND, MPI_COMM_WORLD);

        return global_convergence == 1;
    }

    void Solver::solve(){
        apply_dirichlet_conditions();

        Index rows = p_config.loc_rows;
        Index cols = p_config.loc_cols;
        Real h = s_config.h;
        size_t max_it = s_config.max_it;

        // Vectors to hold ghost rows for communication
        RealVector ghost_row_up(cols, 0.0);
        RealVector ghost_row_down(cols, 0.0);

        Matrix U_old = U;

        size_t it = 0;
        bool converged = false;

        while(!converged && it < max_it){
            // Exchaning ghost rows with neighbours (handle also the case MPI_PROC_NULL)
            MPI_Sendrecv(&U_old(0, 0), cols, MPI_DOUBLE, p_config.rank_down, 0,
                        ghost_row_up.data(), cols, MPI_DOUBLE, p_config.rank_up, 0,
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Sendrecv(&U_old(rows-1, 0), cols, MPI_DOUBLE, p_config.rank_up, 0,
                        ghost_row_down.data(), cols, MPI_DOUBLE, p_config.rank_down, 0,
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            // Update local rows using ghost values when the adjacent row lives on another rank.
            #pragma omp parallel for collapse(2)
            for(Index i = 0; i < rows; ++i){
                for(Index j = 1; j < cols - 1; ++j){
                    Index global_i = p_config.start_row + i;
                    if(global_i == 0 || global_i == s_config.N - 1){
                        continue;
                    }

                    Real down_value = (i == 0) ? ghost_row_down[j] : U_old(i - 1, j);
                    Real up_value = (i == rows - 1) ? ghost_row_up[j] : U_old(i + 1, j);

                    U(i, j) = 0.25 * (up_value + down_value + U_old(i, j - 1) + U_old(i, j + 1) + h * h * F(i, j));
                }
            }

            converged = check_convergence(U_old, U);
            U_old = U;
            ++it;
        }

        if(p_config.rank == 0){
            if(converged){
                std::cout << "Converged in " << it << " iterations." << std::endl;
            }
            else{
                std::cout << "Did not converge after " << max_it << " iterations." << std::endl;
            }
        }

    }

    // Possibile aggiornamento: aggiungi parametro local_count in p_config e usalo per evitare di fare 
    void Solver::convert_to_vtk(const std::string& file_name){
        Index cols = p_config.loc_cols; ///< Global number of columns (same for all processes)

        // Gather global matrix on root process
        Matrix U_global;
        if(p_config.rank == 0){
            U_global.resize(s_config.N, s_config.N);
        }
        
        // Gather the local sizes from all processes to compute the displacements for MPI_Gatherv
        std::vector<int> recv_counts;
        std::vector<int> displs;
        if(p_config.rank == 0){
            recv_counts.resize(p_config.size);
            displs.resize(p_config.size);
        }
        
        // Gather the local sizes from all processes
        int local_size = static_cast<int>(p_config.loc_rows * cols);
        MPI_Gather(&local_size, 1, MPI_INT, recv_counts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Compute displacements for MPI_Gatherv
        if(p_config.rank == 0){
            displs[0] = 0;
            for(int i = 1; i < p_config.size; ++i){
                displs[i] = displs[i-1] + recv_counts[i-1];
            }
        }

        // Gather the local matrices into the global matrix on the root process
        MPI_Gatherv(U.data(), local_size, MPI_DOUBLE, U_global.data(), recv_counts.data(), displs.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // Root process writes the global matrix to a VTK file
        if(p_config.rank == 0){
            std::ofstream vtk_file(file_name);
            if(!vtk_file.is_open()){
                throw std::runtime_error("Could not open file " + file_name + " for writing .vtk!");
            }

            vtk_file << "# vtk DataFile Version 3.0\n";
            vtk_file << "Laplace solution\n";
            vtk_file << "ASCII\n";
            vtk_file << "DATASET STRUCTURED_POINTS\n";
            vtk_file << "DIMENSIONS " << s_config.N << " " << s_config.N << " 1\n";
            vtk_file << "ORIGIN 0 0 0\n";
            vtk_file << "SPACING " << s_config.h << " " << s_config.h << " 1\n";
            vtk_file << "POINT_DATA " << s_config.N * s_config.N << "\n";
            vtk_file << "SCALARS U double 1\n";
            vtk_file << "LOOKUP_TABLE default\n";

            for(Index i = 0; i < s_config.N; ++i){
                for(Index j = 0; j < s_config.N; ++j){
                    vtk_file << U_global(i, j) << "\n";
                }
            }

            vtk_file.close();
        }
    }

    Solver::Solver(const std::string& config_file) {
        s_config; // = Mettere funzione per inizializzare;
        p_config; //  = Mettere funzione per inizializzare
        
        setup(); ///< Initialize the solver based on the provided configurations
    }

    Solver::Solver(const SolverConfig& s, const ParallelConfig& p) : s_config(s), p_config(p){
        setup();/// < Initialize the solver based on the provided configurations
    }

    void Solver::setup(){
        Index rows = p_config.loc_rows;
        Index cols = p_config.loc_cols; 
        s_config.h = 1.0 / (s_config.N - 1); ///< Compute grid spacing based on the number of grid points
        auto h = s_config.h;

        U.resize(rows, cols);
        U.setZero();
        
        F.resize(rows, cols);

        auto f_force = s_config.f;

        /// Initialize the source term F based on source function
        for(Index i = 0; i < rows; ++i){
            Index k = p_config.start_row + i; ///< Global row index
            for(Index j = 0; j < cols; ++j){
                Laplace::Coord coord = {j * h, k * h}; ///< Compute the coordinates of the grid point
                F(i,j) = f_force(coord);
            }
        }
    }

    void Solver::apply_dirichlet_conditions(){
        // Check the consistency of the Dirichlet boundary conditions before applying them
        if(!check_dirichlet_conditions()){
            if(p_config.rank == 0){
                std::cerr << "Inconsistent Dirichlet boundary conditions. Aborting." << std::endl;
            }
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        auto cond1 = s_config.cond1; /// Top boundary condition (y = 1)
        auto cond2 = s_config.cond2; /// Right boundary condition (x = 1)
        auto cond3 = s_config.cond3; /// Bottom boundary condition (y = 0)
        auto cond4 = s_config.cond4; /// Left boundary condition (x = 0)

        Index loc_rows = p_config.loc_rows;
        Index loc_cols = p_config.loc_cols;
        Real h = s_config.h;

        Index global_start_row = p_config.start_row;

        // Update bottom row on its process. Global row 0 corresponds to y = 0.
        if(p_config.rank == 0){
            for(Index j = 1; j < loc_cols -1; ++j){
                Laplace::Coord coord_bottom = {j * h, 0.0};
                U(0, j) = cond3(coord_bottom);
            }
        }
        
        // Update top row on its process. Global row N - 1 corresponds to y = 1.
        if(p_config.rank == p_config.size - 1){
            for(Index j = 1; j < loc_cols -1; ++j){
                Laplace::Coord coord_top = {j * h, 1.0};
                U(loc_rows - 1, j) = cond1(coord_top);
            }
        }
        
        // Update left and right columns on their processes
        for(Index i = 0; i < loc_rows; ++i){
            Index global_i = global_start_row + i;

            Laplace::Coord coord_left = {0.0, (global_i) * h};
            Laplace::Coord coord_right = {1.0, (global_i) * h};
            U(i, 0) = cond4(coord_left);
            U(i, loc_cols - 1) = cond2(coord_right);
        }
    }

    bool Solver::check_dirichlet_conditions(){
        int consistent = 1;
        if(p_config.rank == 0){
            auto cond1 = s_config.cond1; /// Top boundary condition
            auto cond2 = s_config.cond2; /// Right boundary condition
            auto cond3 = s_config.cond3; /// Bottom boundary condition
            auto cond4 = s_config.cond4; /// Left boundary condition

            Laplace::Coord top_left = {0.0, 1.0};
            Laplace::Coord top_right = {1.0, 1.0};
            Laplace::Coord bottom_left = {0.0, 0.0};
            Laplace::Coord bottom_right = {1.0, 0.0};

            Real tol = 1e-10; /// Tolerance for checking consistency of boundary conditions
            if(std::abs(cond1(top_left) - cond4(top_left)) > tol){
                std::cerr << "Inconsistent boundary conditions at top-left corner" << std::endl;
                consistent = 0;
            }
            if(std::abs(cond1(top_right) - cond2(top_right)) > tol){
                std::cerr << "Inconsistent boundary conditions at top-right corner" << std::endl;
                consistent = 0;
            }
            if(std::abs(cond3(bottom_left) - cond4(bottom_left)) > tol){
                std::cerr << "Inconsistent boundary conditions at bottom-left corner" << std::endl;
                consistent = 0;
            }
            if(std::abs(cond3(bottom_right) - cond2(bottom_right)) > tol){
                std::cerr << "Inconsistent boundary conditions at bottom-right corner" << std::endl;
                consistent = 0;
            }
        }
        MPI_Bcast(&consistent, 1, MPI_INT, 0, MPI_COMM_WORLD);
        return consistent == 1;
    }
}
