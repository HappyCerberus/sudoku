#include <benchmark/benchmark.h>

template <typename T>
void generic_recursive_find(
    std::vector<std::vector<unsigned>> &result, const std::vector<T *> &elems,
    unsigned size,
    const std::function<bool(const std::vector<T *> &,
                             const std::vector<unsigned> &)> &is_valid_set,
    unsigned consumed, std::vector<unsigned> path, unsigned first_index) {
  for (unsigned i = first_index; i < elems.size() - (size - consumed - 1);
       i++) {
    if ((size - consumed) == 1) {
      std::vector<unsigned> r = path;
      r.push_back(i);
      if (is_valid_set(elems, r)) {
        result.push_back(r);
      }
    } else {
      std::vector<unsigned> new_path = path;
      new_path.push_back(i);
      generic_recursive_find(result, elems, size, is_valid_set, consumed + 1,
                             new_path, i + 1);
    }
  }
}


template <typename T>
void generic_recursive_find(
    std::vector<std::vector<unsigned>> &result, const std::vector<T *> &elems,
    unsigned size,
    std::function<bool(const std::vector<T *> &, const std::vector<unsigned> &)>
    is_valid_set) {
  generic_recursive_find(result, elems, size, is_valid_set, 0,
                         std::vector<unsigned>{}, 0);
}

static void BM_RecursiveFind(benchmark::State& state) {
  std::vector<std::vector<unsigned>> result;
  unsigned base_data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::vector<unsigned*> data = {&base_data[0], &base_data[1], &base_data[2],
                               &base_data[3],
                               &base_data[4], &base_data[5], &base_data[6],
                               &base_data[7], &base_data[8]};
  auto f = [](const std::vector<unsigned *> &e, const std::vector<unsigned> &s){
    unsigned sum = 0;
    for (auto v : s) {
      sum += *(e[v]);
    }
    return sum % 3 != 0;
  };
  for (auto _ : state){
    generic_recursive_find<unsigned>(result, data,
                                     static_cast<unsigned>(state.range()), f);
    benchmark::DoNotOptimize(result.data());
    benchmark::ClobberMemory();
  }
}

// Register the function as a benchmark
BENCHMARK(BM_RecursiveFind)->DenseRange(2,5);

template <typename T>
void generic_nonrecursive_find(
    std::vector<std::vector<unsigned>> &result, const std::vector<T *> &elems,
    unsigned size,
    const std::function<bool(const std::vector<T *> &,
                             const std::vector<unsigned> &)> &is_valid_set) {
  std::vector<unsigned> path;
  path.reserve(elems.size());
  path.push_back(0);

  while (true) {
    if (path.size() == size) {
      if (path[size - 1] < elems.size() - 1) {
        ++path[size - 1];
        if (is_valid_set(elems, path)) {
          result.push_back(path);
        }
        continue;
      }
    } else {
      for (unsigned i = static_cast<unsigned>(path.size()); i < size; i++) {
        path.push_back(path[i - 1] + 1);
      }
      if (is_valid_set(elems, path)) {
        result.push_back(path);
      }
      continue;
    }

    // if (path.size() == size && path[size-1] == elemens.size()-1)
    while (!path.empty() &&
           path[path.size() - 1] == elems.size() - 1 - (size - path.size())) {
      path.pop_back();
    }
    if (path.empty())
      break;
    ++path[path.size() - 1];
  }
}

// Define another benchmark
static void BM_NonRecursiveFind(benchmark::State& state) {
  std::vector<std::vector<unsigned>> result;
  unsigned base_data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::vector<unsigned*> data = {&base_data[0], &base_data[1], &base_data[2],
                                 &base_data[3],
                                 &base_data[4], &base_data[5], &base_data[6],
                                 &base_data[7], &base_data[8]};
  auto f = [](const std::vector<unsigned *> &e, const std::vector<unsigned> &s){
    unsigned sum = 0;
    for (auto v : s) {
      sum += *(e[v]);
    }
    return sum % 3 == 0;
  };
  for (auto _ : state){
    generic_nonrecursive_find<unsigned>(result, data,
                                     static_cast<unsigned>(state.range()), f);
    benchmark::DoNotOptimize(result.data());
    benchmark::ClobberMemory();
  }
}

BENCHMARK(BM_NonRecursiveFind)->DenseRange(2,5);

BENCHMARK_MAIN();