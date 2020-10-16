#include <vector>
#include <random>
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
    state.counters["n"] = state.range(0);
    state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE(DotProduct, float)->RangeMultiplier(2)->Range(1<<3, 1<<18)->Complexity(benchmark::oN)->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE(DotProduct, double)->RangeMultiplier(2)->Range(1<<3, 1<<18)->Complexity(benchmark::oN)->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE(DotProduct, long double)->RangeMultiplier(2)->Range(1<<3, 1<<18)->Complexity(benchmark::oN)->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();