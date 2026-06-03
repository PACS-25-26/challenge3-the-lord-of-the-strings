# Challenge 3: Results and Performance Analysis

Let's have a look at the visual and numerical results of the code.

### Visual validation

Let's see the plots of the results of the homogeneous and non-homogeneous solution. After computing the solution, the visualization can be done with the command

```cpp
paraview solution.vtk &
```

![[Par_homo.png]]
![[Par_nonhomo.png]]
./
The exact solution are:
- $\sin(2\pi x)\sin(2\pi y)$ for the homogeneous case
- $\exp(x+y) + \sin(\pi x)\cos(2\pi y)$ for the non homogeneous case

### Numerical convergence

The spatial discretization error is evaluated in the $L_2$ norm against the analytical solution, in particular with the error defined as
$$
h \sqrt{\sum_{i,j}\Big( f(i,j) - u(x_{i},x_{j})\Big)^2}
$$
Given that we employed a second-order central difference scheme, the expected theoretical order of convergence is $O(h^2)$. The slope of the computed error decays quadratically as the mesh size $h$ decreases, fully validating the accuracy of the algorithm. In this example, the output is related to the homogeneous case

```
N       Error
16      0.00737541
32      0.00171521
64      0.000414652
128     0.000101998
```

We made a simple Matlab script to visualize the order of convergence

![[Pasted image 20260603120944.png]]
```
=====================================================
Experimental Slope, homogeneous (Order): 2.0039
=====================================================
=====================================================
Experimental Slope, non-homogeneous (Order): 2.0039
=====================================================
```

It matches almost perfectly.

### Benchmarking results

We used the Google Benchmark library to numerically visualize the speedup given by OpenMP: in particular, we tested the function `solve()` with 1 thread (sequential) and 8 thread (parallel).
Note: to see the effective results, we need to build the library without DEBUG, launching the command from the build library

```cpp
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Release ..
```

```cpp
Benchmark                        Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------
BM_SolverSequential/16       0.480 ms        0.480 ms         1338 threads=1
BM_SolverSequential/32        5.74 ms         5.74 ms          119 threads=1
BM_SolverSequential/64        84.4 ms         84.4 ms            8 threads=1
BM_SolverSequential/128       1295 ms         1294 ms            1 threads=1
BM_SolverSequential/256      20421 ms        20414 ms            1 threads=1
BM_SolverOpenMP/16            1.07 ms         1.07 ms          621 threads=8
BM_SolverOpenMP/32            7.07 ms         6.91 ms           86 threads=8
BM_SolverOpenMP/64            63.8 ms         63.7 ms           12 threads=8
BM_SolverOpenMP/128            594 ms          591 ms            1 threads=8
BM_SolverOpenMP/256           8498 ms         8360 ms            1 threads=8
```

The results demonstrate that OpenMP make the calculations almost two/three times faster than the sequential version, when we use enough discretization.