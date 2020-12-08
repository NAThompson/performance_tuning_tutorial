#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <execution>
#include <chrono>
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
    state.SetBytesProcessed(2*int64_t(state.range(0))*sizeof(Real)*int64_t(state.iterations()));
    state.counters["n"] = state.range(0);
    state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE(DotProduct, float)->RangeMultiplier(2)->Range(1<<3, 1<<22)->Complexity(benchmark::oN)->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE(DotProduct, double)->RangeMultiplier(2)->Range(1<<6, 1<<22)->Complexity(benchmark::oN)->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE(DotProduct, long double)->RangeMultiplier(2)->Range(1<<3, 1<<18)->Complexity()->Unit(benchmark::kMicrosecond);

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
        auto start = std::chrono::high_resolution_clock::now();
        std::sort(a.begin(), a.end());
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed_seconds.count());
        benchmark::DoNotOptimize(a[0]);
        std::shuffle(a.begin(), a.end(), rd);
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE(Sort, double)->RangeMultiplier(2)->Range(1<<5, 1<<16)->Complexity()->Unit(benchmark::kMicrosecond)->UseManualTime();

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
        auto start = std::chrono::high_resolution_clock::now();
        std::sort(std::execution::par_unseq, a.begin(), a.end());
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed_seconds.count());
        benchmark::DoNotOptimize(a[0]);
        std::shuffle(a.begin(), a.end(), rd);
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE(SortParallel, double)->RangeMultiplier(2)->Range(1<<5, 1<<16)->Complexity()->Unit(benchmark::kMicrosecond)->UseManualTime();


template <typename T>
int64_t interpolation_search(T arr[], int64_t size, T key)
{

    int64_t low = 0;
    int64_t high = size - 1;
    int64_t mid;

    while ((arr[high] != arr[low]) && (key >= arr[low]) && (key <= arr[high])) {
        mid = low + ((key - arr[low]) * (high - low) / (arr[high] - arr[low]));

        if (arr[mid] < key)
            low = mid + 1;
        else if (key < arr[mid])
            high = mid - 1;
        else
            return mid;
    }

    return low;
}

template<class Real>
void InterpolationSearch(benchmark::State& state)
{
    std::vector<Real> a(state.range(0));
    std::random_device rd;
    std::uniform_real_distribution<Real> unif(-1,1);
    for (size_t i = 0; i < a.size(); ++i)
    {
        a[i] = unif(rd);
    }
    std::sort(a.begin(), a.end());

    for (auto _ : state)
    {
        int64_t i = interpolation_search(a.data(), a.size(), unif(rd));
        benchmark::DoNotOptimize(i);
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE(InterpolationSearch, double)->RangeMultiplier(2)->Range(1<<5, 1<<18)->Complexity()->Unit(benchmark::kMicrosecond);

template<class Real>
void BinarySearch(benchmark::State& state)
{
    std::vector<Real> a(state.range(0));
    std::random_device rd;
    std::uniform_real_distribution<Real> unif(-1,1);
    for (size_t i = 0; i < a.size(); ++i)
    {
        a[i] = unif(rd);
    }
    std::sort(a.begin(), a.end());
    for (auto _ : state)
    {
        auto it = std::upper_bound(a.begin(), a.end(), unif(rd));
        benchmark::DoNotOptimize(it);
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE(BinarySearch, double)->RangeMultiplier(2)->Range(1<<5, 1<<18)->Complexity()->Unit(benchmark::kMicrosecond);


BENCHMARK_MAIN();
