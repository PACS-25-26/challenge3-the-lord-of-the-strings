#include "Alias.hpp"
#include "Solver.hpp"
#include "DataManager.hpp"
#include <algorithm>
#include <benchmark/benchmark.h>
#include <omp.h>
#include <string>
#include <vector>

namespace {
int openmp_threads = 1;
Laplace::SolverConfig make_solver_config(int N)
{
    std::string file_path = "gandalf_the_white.cfg";
    std::vector<std::string> raw_data = Laplace::read_data(file_path);
    Laplace::SolverConfig s_config = Laplace::process_data(raw_data);

    s_config.N = N;
    s_config.h = 1.0 / (N - 1);

    return s_config;
}

Laplace::ParallelConfig make_single_process_config(int N)
{
    Laplace::ParallelConfig p_config;
    p_config.rank = 0;
    p_config.size = 1;
    p_config.start_row = 0;
    p_config.loc_rows = N;
    p_config.loc_cols = N;
    p_config.rank_up = MPI_PROC_NULL;
    p_config.rank_down = MPI_PROC_NULL;

    return p_config;
}

static void BM_SolverWithThreads(benchmark::State& state, int thread_count)
{
    int N = state.range(0);

    omp_set_num_threads(thread_count);

    Laplace::SolverConfig s_config = make_solver_config(N);
    Laplace::ParallelConfig p_config = make_single_process_config(N);

    for (auto _ : state) {
        Laplace::Solver solver(s_config, p_config);
        solver.solve();
    }

    state.counters["threads"] = thread_count;
}

static void BM_SolverSequential(benchmark::State& state)
{
    BM_SolverWithThreads(state, 1);
}

static void BM_SolverOpenMP(benchmark::State& state)
{
    BM_SolverWithThreads(state, openmp_threads);
}
}

BENCHMARK(BM_SolverSequential)->RangeMultiplier(2)->Range(16, 64)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_SolverOpenMP)->RangeMultiplier(2)->Range(16, 64)->Unit(benchmark::kMillisecond);

int main(int argc, char** argv)
{
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    omp_set_dynamic(0);
    openmp_threads = std::max(1, omp_get_max_threads());

    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv)) {
        MPI_Finalize();
        return 1;
    }

    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();

    MPI_Finalize();
    return 0;
}
