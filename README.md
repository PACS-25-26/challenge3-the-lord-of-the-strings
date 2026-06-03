[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/tKSbaXxd)

# Challenge 3 - A matrix–free parallel solver for the Laplace equation

Developed by Elena Vera Cassano and David Laurenti, June 2026. References of the requests and of the theoretical background are on the PDF file.
## Quick overview

This project provides a C++17 solver for the two-dimensional Laplace/Poisson problem on the unit square, using a finite difference discretization and an iterative Jacobi method. In particular the problem has the following generalised form:

$$
	\begin{cases}
	&-\Delta u = f(x), & in \ \Omega=(0,1)²
	\\
	&u = h_1(x), & on \ \{y = 1\},
	\\
	&u = h_2(x), & on \ \{x = 1\},
	\\
	&u = h_3(x), & on \ \{y = 0\},
	\\
	&u = h_4(x), & on \ \{x = 0\},
	\end{cases}
$$

Developed as part of the PACS (Advanced Programming for Scientific Computing) course, the work focuses on the implementation of a parallel solver that makes use of:

- **MPI**, for distributed-memory parallelization through a row-wise domain decomposition.

- **OpenMP**, for shared-memory acceleration of local matrix operations.

The final solution is written in `.vtk` format, so it can be visualized with standard scientific visualization tools (for example, Paraview).

## Structure

```cpp

include/

Alias.hpp // type aliases, Eigen matrix type, MPI/OpenMP includes

DataManager.hpp // declaration of configuration structures and parser utilities

Solver.hpp // declaration of the Jacobi solver class

  

src/

DataManager.cpp // reading and processing of configuration files

Solver.cpp // implementation of the parallel Jacobi algorithm

  

test/

convergence_test.cpp // convergence/error test for different grid sizes

seqpar_test.cpp // benchmark between sequential and OpenMP execution

  

main.cpp // main executable

CMakeLists.txt // build configuration

gandalf_the_white.cfg // first test case

gandalf_the_grey.cfg // second test case

Challenge25-26-3.pdf // assignment text

```  

## Features

- **Finite difference solver**: the equation is discretized on a uniform grid of `N x N` points on the domain `[0,1] x [0,1]`.

- **Jacobi iteration**: the solution is updated iteratively until the selected tolerance is reached or the maximum number of iterations is exceeded.

- **Dirichlet boundary conditions**: the four sides of the domain are read from the configuration file and applied before the iterative procedure.

- **Runtime function parsing**: exact solution, forcing term and boundary conditions are provided as strings and evaluated through `muParser`.

- **MPI parallelization**: the domain is split by rows among the available processes. Adjacent ranks communicate ghost rows using MPI communication.

- **OpenMP acceleration**: local updates and convergence checks are parallelized using OpenMP directives.

- **Error computation**: when an exact solution is available, the solver can compute the numerical error in L2 norm.

- **VTK output**: the global solution is gathered on the root process and exported to `solution.vtk`.

## Configuration files

The solver reads a `.cfg` file containing the problem data. The expected order is:

```txt

u_ex

f

cond1 # top boundary, y = 1

cond2 # right boundary, x = 1

cond3 # bottom boundary, y = 0

cond4 # left boundary, x = 0

N

tol

max_it

```

The solver provides a suitable **parser** that ensures memory safety also for shared memory parallelisation.

Two test cases are already provided:

- `gandalf_the_white.cfg`, that provides a solution for the problem with homogeneous boundary conditions.

- `gandalf_the_grey.cfg`, with non-homogeneous boundary conditions.

## How to run

### Prerequisites

The project needs a C++17 compiler and the following libraries:

- **MPI**

- **OpenMP**

- **Eigen3**

- **muParser**

- **Google Benchmark**, automatically downloaded by CMake through `FetchContent`

On Ubuntu:

```bash
sudo apt update

sudo apt install build-essential cmake libopenmpi-dev openmpi-bin libeigen3-dev libmuparser-dev
```

On macOS (Homebrew):

```bash
brew install cmake open-mpi eigen muparser
```

### Compilation

Create and enter the build directory:

```bash
cmake -S . -B build
cmake --build build
```

### Execution

It is possible to compile either one of the following targets:
- `solver_run`
- `convergence_test`
- `seqpar_test`

Or all of them through the command

```bash
make [target]
```

Then it is possible to launch directly the desired target, choosing the openMP threads number and executing with the use of MPI, also specifying the number of processes. 
The configuration file must be added as `argv[1]`.

```bash
OMP_NUM_THREADS=2 mpiexec -n 4 ./build/solver_run gandalf_the_white.cfg
```

## Tests and benchmarks

### Convergence test

The convergence executable solves the problem for increasing grid sizes and prints the error:

```bash
cmake --build build --target convergence_white

cmake --build build --target convergence_grey
```

### Sequential/OpenMP benchmark

The benchmark executable compares the solver using one OpenMP thread against the available OpenMP parallel execution. It reads `gandalf_the_white.cfg`, so it can be launched with:

```bash
./build/seqpar_test
```

## Example output

Running the solver prints the convergence status:

```txt

Converged in 12345 iterations.

```

After the execution, the file `solution.vtk` is created and can be opened for visualization of the computed solution.

**Note**: the number of iterations and the final error depend on the selected configuration file, grid size, tolerance and number of processes.

**Note**: the available file RESULTS.md provides a more complete overview of the results obtained using Google Benchmark, analysing both the case of homogeneous and non-homogeneous boundary conditions.