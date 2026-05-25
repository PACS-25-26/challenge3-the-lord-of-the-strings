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
    
}