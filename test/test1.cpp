#include <benchmark/benchmark.h>
#include <Eigen/Dense>

using Matrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

// Benchmark function
static void BM_MatrixAllocation(benchmark::State& state) {
    int N = state.range(0); 

    for (auto _ : state) {
        Matrix U(N, N);
        U.setZero();
        
        benchmark::DoNotOptimize(U.data());
    }
}

BENCHMARK(BM_MatrixAllocation)->Arg(100)->Arg(500)->Arg(1000);

BENCHMARK_MAIN();