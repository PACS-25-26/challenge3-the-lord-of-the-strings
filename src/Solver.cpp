#include "Solver.hpp"

bool Laplace::Solver::check_convergence(const Matrix& U_old, const Matrix& U_new){
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

void Laplace::Solver::solve(){
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
        MPI_Sendrecv(&U_old(0, 0), cols, MPI_DOUBLE, p_config.rank_up, 0,
                     ghost_row_down.data(), cols, MPI_DOUBLE, p_config.rank_down, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Sendrecv(&U_old(rows-1, 0), cols, MPI_DOUBLE, p_config.rank_down, 0,
                     ghost_row_up.data(), cols, MPI_DOUBLE, p_config.rank_up, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        // Update U using the Jacobi method
        #pragma omp parallel for collapse(2)
        for(Index i = 1; i < rows - 1; ++i){
            for(Index j = 1; j < cols - 1; ++j){
                U(i, j) = 0.25 * (U_old(i - 1, j) + U_old(i + 1, j) + U_old(i, j - 1) + U_old(i, j + 1) - h * h * F(i, j));
            }
        }
        
        // Update up ghost row
        if(p_config.rank_up != MPI_PROC_NULL){
            #pragma omp parallel for
            for(Index j = 1; j < cols-1; ++j){
                U(0, j) = 0.25 * (ghost_row_up[j] + U_old(1, j) + U_old(0, j-1) + U_old(0, j+1) - h*h * F(0, j));
            }
        }

        // Update down ghost row
        if(p_config.rank_down != MPI_PROC_NULL){
            #pragma omp parallel for
            for(Index j = 1; j < cols-1; ++j){
                U(rows-1, j) = 0.25 * (U_old(rows-2, j) + ghost_row_down[j] + U_old(rows-1, j-1) + U_old(rows-1, j+1) - h*h * F(rows-1, j));
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