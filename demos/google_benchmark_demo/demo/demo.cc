#include <benchmark/benchmark.h>
#include <spline.h>
#include <vector>

const std::vector<double> X = {0.1, 0.4, 1.2, 1.8, 2.0};  // must be increasing
const std::vector<double> Y = {0.1, 0.7, 0.6, 1.1, 0.9};

static void BM_C2SplineCreation(benchmark::State& state) {
  for (auto _ : state) {
    tk::spline s(X, Y ,tk::spline::cspline);
  }
}
// Register the function as a benchmark
BENCHMARK(BM_C2SplineCreation);

static void BM_C1SplineCreation(benchmark::State& state) {
  for (auto _ : state) {
    tk::spline s(X, Y ,tk::spline::cspline_hermite);
  }
}
// Register the function as a benchmark
BENCHMARK(BM_C1SplineCreation);
