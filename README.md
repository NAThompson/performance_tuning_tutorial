slidenumbers: true

# Performance Tuning

![inline](logo.svg)

Nick Thompson

---

Session 1: Using `perf`

---

# What is it?

 - Performance tools for linux
 - Designed to profile kernel, but can profile userspace apps
 - Sampling based
 - Canonized in linux kernel source code

---

# Installing `perf`: Ubuntu

```bash
$ sudo apt install linux-tools-common
$ sudo apt install linux-tools-generic
$ sudo apt install linux-tools-`uname -r`
```

---

# Installing `perf`: CentOS

```bash
$ yum install perf
```

---

# Installing `perf`: Source build

If your system doesn't have `perf` and you don't have root, do a source build out of the Linux kernel.

```bash
$ git clone https://github.com/torvalds/linux.git
$ cd linux/tools/perf;
$ uname -r
5.8.0-48-generic
$ git checkout v5.8
$ make
$ ./perf
```

---

# `perf` mwe

```bash
$ perf stat ls
data  Desktop  Documents  Downloads  Music  Pictures  Public  Templates  TIS  Videos

 Performance counter stats for 'ls':

              2.78 msec task-clock:u              #    0.094 CPUs utilized          
                 0      context-switches:u        #    0.000 K/sec                  
                 0      cpu-migrations:u          #    0.000 K/sec                  
               283      page-faults:u             #    0.102 M/sec                  
           838,657      cycles:u                  #    0.302 GHz                    
           584,659      instructions:u            #    0.70  insn per cycle         
           128,106      branches:u                #   46.109 M/sec                  
             7,907      branch-misses:u           #    6.17% of all branches        

       0.029630910 seconds time elapsed

       0.000000000 seconds user
       0.003539000 seconds sys
```

---

## Access `perf`:

`perf` is available on Summit (summit.olcf.ornl.gov), Rhea (rhea.ccs.ornl.gov), Cori (cori.nersc.gov), Theta (theta.alcf.anl.gov), and the SNS nodes (analysis.sns.gov)

---

# Why `perf`?

There are lots of great performance analysis tools (Intel VTune, Score-P, tau, cachegrind), but my opinion is that `perf` should be the first tool you reach for.

---

# Why `perf`?

- No fighting for a licence, or install Java runtimes on HPC clusters
- No need to vandalize souce code, or be constrained to work with a set of specific languages
- Text gui, so easy to use in terminal and over `ssh`
- Available on any Linux system, in particular Summit, SNS nodes
- Not tied to x86
- Samples rather than models your program

---

# Why not `perf`?

- Text gui, so fancy graphics must be generated by post-processing
- Awkward to simultaneously monitor multiple MPI ranks
- High levels of education needed to make `perf` output actionable
- *Only* available on Linux
- Significant limitations when profiling GPUs

---

### `src/mwe.cpp`

```cpp
#include <iostream>
#include <vector>

double dot_product(double* a, double* b, size_t n) {
    double d = 0;
    for (size_t i = 0; i < n; ++i) {
        d += a[i]*b[i];
    }
    return d;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./dot 10\n";
        return 1;
    }
    size_t n = atoi(argv[1]);

    std::vector<double> a(n);
    std::vector<double> b(n);
    for (size_t i = 0; i < n; ++i) {
        a[i] = i;
        b[i] = 1/double(i+3);
    }

    double d = dot_product(a.data(), b.data(), n);
    std::cout << "a.b = " << d << "\n";
}
```

---

# Running the MWE under `perf`

```bash
$ make dot
$ perf stat ./dot 100000000
a.b = 9.99999e+07

 Performance counter stats for './dot 100000000':

          3,782.94 msec task-clock:u              #    1.000 CPUs utilized          
                 0      context-switches:u        #    0.000 K/sec                  
                 0      cpu-migrations:u          #    0.000 K/sec                  
           600,334      page-faults:u             #    0.159 M/sec                  
     5,746,594,234      cycles:u                  #    1.519 GHz                    
     8,601,976,669      instructions:u            #    1.50  insn per cycle         
     1,200,846,522      branches:u                #  317.437 M/sec                  
            10,231      branch-misses:u           #    0.00% of all branches        

       3.784742173 seconds time elapsed

       2.530558000 seconds user
       1.247303000 seconds sys
```

---

# Learning from `perf stat`

- 1.5 instructions/cycle means we're probably CPU bound. (But don't make the mistake of thinking CPU-bound = WINNING)
- Our branch miss rate is really good!

But it's not super informative, nor is it actionable.

---

# Learning from `perf stat`

`perf` is designed for kernel developers, so the `perf stat` defaults are for them.

At ORNL, we're HPC developers, so let's make some changes. What stats do we have available?

---

```
$ perf list
List of pre-defined events (to be used in -e):

  branch-misses                                      [Hardware event]
  cache-misses                                       [Hardware event]
  cache-references                                   [Hardware event]
  instructions                                       [Hardware event]
  task-clock                                         [Software event]

  L1-dcache-load-misses                              [Hardware cache event]
  L1-dcache-loads                                    [Hardware cache event]
  LLC-load-misses                                    [Hardware cache event]
  LLC-loads                                          [Hardware cache event]

  cache-misses OR cpu/cache-misses/                  [Kernel PMU event]
  cache-references OR cpu/cache-references/          [Kernel PMU event]
  power/energy-cores/                                [Kernel PMU event]
  power/energy-pkg/                                  [Kernel PMU event]
  power/energy-ram/                                  [Kernel PMU event]
```

---

# Custom events:

```
perf stat -e instructions,cycles,L1-dcache-load-misses,L1-dcache-loads,LLC-load-misses,LLC-loads ./dot 100000000
a.b = 9.99999e+07

 Performance counter stats for './dot 100000000':

     8,564,368,466      instructions:u            #    1.41  insn per cycle           (49.98%)
     6,060,955,584      cycles:u                                                      (66.65%)
        34,089,080      L1-dcache-load-misses:u   #    0.90% of all L1-dcache hits    (83.34%)
     3,805,929,303      L1-dcache-loads:u                                             (83.32%)
           854,522      LLC-load-misses:u         #   39.87% of all LL-cache hits     (33.31%)
         2,143,437      LLC-loads:u                                                   (33.31%)

       5.045450844 seconds time elapsed

       2.856660000 seconds user
       2.185739000 seconds sys
```

Hmm . . . 40% LL cache miss rate, yet 1.4 instructions/cycle.

This CPU-bound vs memory-bound is a bit complicated . . . 

---


```
$ perf list
  cycle_activity.stalls_l1d_pending                 
       [Execution stalls due to L1 data cache misses]
  cycle_activity.stalls_l2_pending                  
       [Execution stalls due to L2 cache misses]
  cycle_activity.stalls_ldm_pending                 
       [Execution stalls due to memory subsystem]
$ perf stat -e cycle_activity.stalls_ldm_pending,cycle_activity.stalls_l2_pending,cycle_activity.stalls_l1d_pending,cycles ./dot 10000000
a.b = 9.99999e+07

 Performance counter stats for './dot 100000000':

       509,998,525      cycle_activity.stalls_ldm_pending:u                                   
       127,137,070      cycle_activity.stalls_l2_pending:u                                   
        70,555,574      cycle_activity.stalls_l1d_pending:u                                   
     5,708,220,052      cycles:u                                                    

       3.637099623 seconds time elapsed

       2.463966000 seconds user
       1.172459000 seconds sys
```

1/10th of the cycles are stalled due to memory pressure.

Is this CPU-bound or memory-bound?

---

# `perf stat` is great for reporting . . .

But not super actionable.

---

# Get Actionable Data

```
➜  performance_tuning_tutorial git:(master) ✗ perf record -g ./dot 100000000
a.b = 9.99999e+07
[ perf record: Woken up 3 times to write data ]
[ perf record: Captured and wrote 0.735 MB perf.data (5894 samples) ]
➜  performance_tuning_tutorial git:(master) ✗ perf report -g -M intel
```

![inline](figures/perf_report_homescreen.png)

---

# Self and Children

- The `Self` column says how much time was taken within the function.
- The `Children` column says how much time was spent in functions called by the function.

- If the `Children` column value is very near the `Self` column value, that function isn't your hotspot!


---

# Self and Children

If `Self` and `Children` is confusing, just get rid of it:

```bash
$ perf report -g -U -M intel --no-children
```

---

# Disassembly: Navigate to function of interest and hit 'a'

![inline](figures/dot_disassembly.png)

---

# What is happening?????

- If you don't know x86 assembly, I recommend Ray Seyfarth's [Introduction to 64 Bit Assembly Language Programming for Linux and OS X](http://rayseyfarth.com/asm/)

- If you need to look up instructions one at a time, Felix Cloutier's [x64 reference](https://www.felixcloutier.com/x86/) is a great resource.

- If you need to examine how compiler flags interact with generated assembly, try [godbolt](https://godbolt.org).

---

# Detour: System V ABI

- A floating point return value is placed in register `xmm0`.
- The first integer argument is placed in `rdi`
- The second integer argument is placed in `rsi`
- The third integer argument is placed in `rdx`

---

# The default assembly generated by gcc is completely braindead.

See the [gotbolt](https://godbolt.org/z/8qqhGj).

- Superfluous stack writes.
- No AVX instructions

Consequence: Lots of time spent moving data around.

(Note: This default is good for running under a debugger, but somehow the defaults often don't get changed!)

---

# My preferred CPPFLAGS:

```
-g -O3 -ffast-math -fno-finite-math-only -march=native
```

Add `-fno-omit-frame-pointer` for performance analysis.

How does that look on [godbolt](https://godbolt.org/z/4dnfYb)?

Key instruction: `vfmadd132pd`; vectorized fused multiply add on `ymm`/`zmm` registers.

---

```
$ perf stat ./dot 100000000     
a.b = 9.99999e+07

 Performance counter stats for './dot 100000000':

          2,428.06 msec task-clock:u              #    0.998 CPUs utilized          
                 0      context-switches:u        #    0.000 K/sec                  
                 0      cpu-migrations:u          #    0.000 K/sec                  
           390,994      page-faults:u             #    0.161 M/sec                  
     3,651,637,732      cycles:u                  #    1.504 GHz                    
     1,676,766,309      instructions:u            #    0.46  insn per cycle         
       225,636,250      branches:u                #   92.929 M/sec                  
             9,303      branch-misses:u           #    0.00% of all branches        

       2.432163719 seconds time elapsed

       1.511267000 seconds user
       0.915950000 seconds sys
```

1/3rd of the instructions/cycle, yet twice as fast, because it ran ~1/5th the number of instructions.

---

# Mind bogglement

I could not get any compiler to generate AVX-512 fma instructions.

Is Intel giving up on this?

---

# Beautiful assembly:

![inline](figures/ymm_dot_product.png)

---

# perf report commands

- `k`: Show line numbers of source code
- `o`: Show instruction number
- `t`: Switch between percentage and samples
- `J`: Number of jump sources on target; number of places that can jump here.
- `s`: Hide/Show source code
- `h`: Show options

---

# perf gotchas

- perf sometimes attributes the time in a single instruction to the *next* instruction.


---

# perf gotchas

```bash
     │         if (absx < 1)
7.76 │       ucomis xmm1,QWORD PTR [rbp-0x20]
0.95 │     ↓ jbe    a6
1.82 │       movsd  xmm0,QWORD PTR ds:0x46a198
0.01 │       movsd  xmm1,QWORD PTR ds:0x46a1a0
0.01 |       movsd  xmm2,QWORD PTR ds:0x46a100 
```

Hmm, so moving data into `xmm1` and `xmm2` is 182x faster than moving data into `xmm0` . . .

Looks like a misattribution of the `jbe`.


---

To determine if your CPU has `ymm` registers, check for avx instruction support:

```bash
$ lscpu | grep avx
Flags:
fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush
dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx rdtscp lm constant_tsc arch_perfmon
pebs bts rep_good nopl xtopology nonstop_tsc aperfmperf eagerfpu pni pclmulqdq dtes64
monitor ds_cpl vmx smx est tm2 ssse3 cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic popcnt
tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm epb tpr_shadow vnmi 
flexpriority ept vpid fsgsbase smep erms xsaveopt dtherm ida arat pln pts
```

or (on Centos)

```bash
$ cat /proc/cpuinfo | grep avx
```

---

Break?

---

# Challenges we need to overcome

- Our MWE spent fully half its time initializing data. That's not very interesting.
- We could only specify one vector length at a time. What if we'd written a performance bug that induced quadratic scaling?
- How do we attach to a running process? or different MPI ranks?
- How do we visualize complicated `perf` output to get an understanding of a complex program?

The first two problems can be solved with google/benchmark.

---

## A [google/benchmark](https://github.com/google/benchmark/) [example](https://github.com/boostorg/math/blob/develop/reporting/performance/chebyshev_clenshaw.cpp):

```bash
$ ./reporting/performance/chebyshev_clenshaw.x --benchmark_filter=^ChebyshevClenshaw
2020-10-16T15:36:34-04:00
Running ./reporting/performance/chebyshev_clenshaw.x
Run on (16 X 2300 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 256 KiB (x8)
  L3 Unified 16384 KiB (x1)
Load Average: 2.49, 2.29, 2.09
----------------------------------------------------------------------------
Benchmark                                  Time             CPU   Iterations
----------------------------------------------------------------------------
ChebyshevClenshaw<double>/2            0.966 ns        0.965 ns    637018028
ChebyshevClenshaw<double>/4             1.69 ns         1.69 ns    413440355
ChebyshevClenshaw<double>/8             4.26 ns         4.25 ns    161924589
ChebyshevClenshaw<double>/16            13.3 ns         13.3 ns     52107759
ChebyshevClenshaw<double>/32            39.4 ns         39.4 ns     17071255
ChebyshevClenshaw<double>/64             108 ns          108 ns      6438439
ChebyshevClenshaw<double>/128            246 ns          245 ns      2852707
ChebyshevClenshaw<double>/256            522 ns          521 ns      1316359
ChebyshevClenshaw<double>/512           1100 ns         1100 ns       640076
ChebyshevClenshaw<double>/1024          2180 ns         2179 ns       311353
ChebyshevClenshaw<double>/2048          4499 ns         4496 ns       152754
ChebyshevClenshaw<double>/4096          9086 ns         9081 ns        79369
ChebyshevClenshaw<double>_BigO          2.27 N          2.26 N
ChebyshevClenshaw<double>_RMS              4 %             4 %
```

---

## Goals for google/benchmark

- Empirically determine asymptotic complexity; is it $$\mathcal{O}(N)$$, $$\mathcal{O}(N^2)$$, or $$\mathcal{O}(\log(N))$$?
- Test inputs of different lengths
- Test different types (`float`, `double`, `long double`)
- Dominate the runtime with interesting and relevant operations so our `perf` traces are more meaningful.

---

## Installation

- Grab a [release tarball](https://github.com/google/benchmark/releases)
- `pip install google-benchmark`
- `brew install google-benchmark`
- `spack install benchmark`

---

## Installation

Source build

```bash
$ git clone https://github.com/google/benchmark.git
$ cd benchmark && mkdir build && cd build
build$ cmake -DCMAKE_BUILD_TYPE=Release -DBENCHMARK_ENABLE_TESTING=OFF ../ -G Ninja
build$ ninja
build$ sudo ninja install
```

---

## Example: `benchmarks/bench.cpp`

```cpp
#include <vector>
#include <random>
#include <benchmark/benchmark.h>

template<class Real>
void DotProduct(benchmark::State& state) {
    std::vector<Real> a(state.range(0));
    std::vector<Real> b(state.range(0));
    std::random_device rd;
    std::uniform_real_distribution<Real> unif(-1,1);
    for (size_t i = 0; i < a.size(); ++i) {
        a[i] = unif(rd);
        b[i] = unif(rd);
    }

    for (auto _ : state) {
        benchmark::DoNotOptimize(dot_product(a.data(), b.data(), a.size()));
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE(DotProduct, float)->RangeMultiplier(2)->Range(1<<3, 1<<18)->Complexity();
BENCHMARK_TEMPLATE(DotProduct, double)->DenseRange(8, 1024*1024, 512)->Complexity();
BENCHMARK_TEMPLATE(DotProduct, long double)->RangeMultiplier(2)->Range(1<<3, 1<<18)->Complexity(benchmark::oN);

BENCHMARK_MAIN();
```

---

Instantiate a benchmark on type float:

```cpp
BENCHMARK_TEMPLATE(DotProduct, float);
```

Test on vectors of length 8, 16, 32,.., 262144:

```
->RangeMultiplier(2)->Range(1<<3, 1<<18)
```

Regress the performance data against $$\mathcal{O}(\log(n)), \mathcal{O}(n), \mathcal{O}(n^2), \mathcal{O}(n^3)$$:

```
->Complexity();
```

---

Force regression against $$\mathcal{O}(n)$$:

```
->Complexity(benchmark::oN);
```

Repeat the calculation until confidence in the runtime is obtained:

```cpp
for (auto _ : state) { ... }
```

Make sure the compiler doesn't elide these instructions:

```cpp
benchmark::DoNotOptimize(dot_product(a.data(), b.data(), a.size()));
```

---

## google/benchmark party tricks: Visualize complexity

Set a counter to the length of the vector:

```
state.counters["n"] = state.range(0);
```

Then get the output as CSV:

```
benchmarks$ ./dot_bench --benchmark_format=csv
```

Finally, copy-paste the console output into [scatterplot.online](https://scatterplot.online/)

---

![inline](figures/benchmark_linear_complexity.png)

---

## Multithreading with OpenMP:

