#include <vector>
#include <random>
#include <algorithm>
#include <execution>
#include <benchmark/benchmark.h>

template<typename Real>
Real dot_product(Real* a, Real* b, unsigned long long n) {
    Real d = 0;
    for (unsigned long long i = 0; i < n; ++i) {
        d += a[i]*b[i];
    }
    return d;
}


template<class Real>
void DotProduct(benchmark::State& state)
{
    std::vector<Real> a(state.range(0));
    std::vector<Real> b(state.range(0));
    std::random_device rd;
    std::uniform_real_distribution<Real> unif(-1,1);
    for (size_t i = 0; i < a.size(); ++i)
    {
        a[i] = unif(rd);
        b[i] = unif(rd);
    }

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(dot_product(a.data(), b.data(), a.size()));
    }
    state.SetBytesProcessed(state.range(0)*sizeof(Real));
    state.counters["n"] = state.range(0);
    state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE(DotProduct, float)->RangeMultiplier(2)->Range(1<<3, 1<<18)->Complexity(benchmark::oN)->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE(DotProduct, double)->DenseRange(1024, 1024*1024, 1024)->Complexity(benchmark::oN)->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE(DotProduct, long double)->RangeMultiplier(2)->Range(1<<3, 1<<18)->Complexity(benchmark::oN)->Unit(benchmark::kMicrosecond);

template<class Real>
void Sort(benchmark::State& state)
{
    std::vector<Real> a(state.range(0));
    std::random_device rd;
    std::uniform_real_distribution<Real> unif(-1,1);
    for (size_t i = 0; i < a.size(); ++i)
    {
        a[i] = unif(rd);
    }

    for (auto _ : state)
    {
        std::sort(a.begin(), a.end());
        benchmark::DoNotOptimize(a[0]);
        state.PauseTiming();
        std::shuffle(a.begin(), a.end(), rd);
        state.ResumeTiming();
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE(Sort, double)->RangeMultiplier(2)->Range(1<<7, 1<<19)->Complexity()->Unit(benchmark::kMicrosecond)->UseRealTime();

template<class Real>
void SortParallel(benchmark::State& state)
{
    std::vector<Real> a(state.range(0));
    std::random_device rd;
    std::uniform_real_distribution<Real> unif(-1,1);
    for (size_t i = 0; i < a.size(); ++i)
    {
        a[i] = unif(rd);
    }

    for (auto _ : state)
    {
        std::sort(std::execution::par_unseq, a.begin(), a.end());
        benchmark::DoNotOptimize(a[0]);
        state.PauseTiming();
        std::shuffle(a.begin(), a.end(), rd);
        state.ResumeTiming();
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE(SortParallel, double)->RangeMultiplier(2)->Range(1<<7, 1<<19)->Complexity()->Unit(benchmark::kMicrosecond)->UseRealTime();


BENCHMARK_MAIN();