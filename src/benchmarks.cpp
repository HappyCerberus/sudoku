#include <benchmark/benchmark.h>
#include <functional>
#include <random>

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

template <typename T>
void generic_nonrecursive_find_v2(
    std::vector<unsigned> &result, const std::vector<T *> &elems,
    unsigned size,
    const std::function<bool(const std::vector<T *> &,
                             const std::vector<unsigned> &)> &is_valid_set) {
  std::vector<unsigned> path;
  path.reserve(size);
  path.push_back(0);

  while (true) {
    if (path.size() == size) {
      if (path[size - 1] < elems.size() - 1) {
        ++path[size - 1];
        if (is_valid_set(elems, path)) {
          for (size_t i = 0; i < size; i++)
            result.push_back(path[i]);
        }
        continue;
      }
    } else {
      for (unsigned i = static_cast<unsigned>(path.size()); i < size; i++) {
        path.push_back(path[i - 1] + 1);
      }
      if (is_valid_set(elems, path)) {
        for (size_t i = 0; i < size; i++)
          result.push_back(path[i]);
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

template <typename Fnc> void BM_find(benchmark::State &state, Fnc &&call) {
  std::vector<std::vector<unsigned>> result;
  unsigned base_data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::vector<unsigned *> data = {&base_data[0], &base_data[1], &base_data[2],
                                  &base_data[3], &base_data[4], &base_data[5],
                                  &base_data[6], &base_data[7], &base_data[8]};
  auto f = [](const std::vector<unsigned *> &e,
              const std::vector<unsigned> &s) {
    unsigned sum = 0;
    for (auto v : s) {
      sum += *(e[v]);
    }
    return sum % 3 == 0;
  };
  for (auto _ : state) {
    call(result, data, static_cast<unsigned>(state.range()), f);
    benchmark::DoNotOptimize(result.data());
    benchmark::ClobberMemory();
  }
}

template <typename Fnc> void BM_find_v2(benchmark::State &state, Fnc &&call) {
  std::vector<unsigned> result;
  unsigned base_data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::vector<unsigned *> data = {&base_data[0], &base_data[1], &base_data[2],
                                  &base_data[3], &base_data[4], &base_data[5],
                                  &base_data[6], &base_data[7], &base_data[8]};
  auto f = [](const std::vector<unsigned *> &e,
              const std::vector<unsigned> &s) {
    unsigned sum = 0;
    for (auto v : s) {
      sum += *(e[v]);
    }
    return sum % 3 == 0;
  };
  for (auto _ : state) {
    call(result, data, static_cast<unsigned>(state.range()), f);
    benchmark::DoNotOptimize(result.data());
    benchmark::ClobberMemory();
  }
}

BENCHMARK_CAPTURE(
    BM_find, recursive,
    static_cast<void (*)(std::vector<std::vector<unsigned>> &,
                         const std::vector<unsigned *> &, unsigned size,
                         std::function < bool(const std::vector<unsigned *> &,
                                              const std::vector<unsigned> &)>)>(
        generic_recursive_find<unsigned>))
    ->DenseRange(2, 5);
BENCHMARK_CAPTURE(BM_find, non_recursive, generic_nonrecursive_find<unsigned>)
    ->DenseRange(2, 5);
BENCHMARK_CAPTURE(BM_find_v2, non_recursive,
                  generic_nonrecursive_find_v2<unsigned>)
->DenseRange(2, 5);

// test the impact of monitoring change in a sudoku square
// test multiple passes (as an argument of the benchmark) over the data
// before checking if changed

class RandomData : public benchmark::Fixture {
public:
  void SetUp(benchmark::State &state) override {
    size_t size = static_cast<size_t>(state.range());
    random_numbers_.reserve(size);
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_int_distribution<unsigned> distr(
        0, std::numeric_limits<unsigned>::max());
    for (size_t i = 0; i < size; i++) {
      random_numbers_.push_back(distr(generator));
    }
  }

  void TearDown(benchmark::State &) override { random_numbers_.clear(); }

protected:
  std::vector<unsigned> random_numbers_;
};

BENCHMARK_DEFINE_F(RandomData, TrackChangeOnce)(benchmark::State &state) {
  for (auto _ : state) {
    bool changed = false;
    unsigned v = this->random_numbers_[0];
    for (size_t i = 0; i < this->random_numbers_.size(); i++) {
      v &= this->random_numbers_[i];
      benchmark::DoNotOptimize(v);
    }
    if (v != this->random_numbers_[0]) {
      changed = true;
    }
    benchmark::DoNotOptimize(changed);
  }
}

BENCHMARK_DEFINE_F(RandomData, TrackChangeIncremental)
(benchmark::State &state) {
  for (auto _ : state) {
    bool changed = false;
    unsigned v = this->random_numbers_[0];
    for (size_t i = 0; i < this->random_numbers_.size(); i++) {
      if (v != (v & this->random_numbers_[i])) {
        changed = true;
      }
      v &= this->random_numbers_[i];
      benchmark::DoNotOptimize(v);
    }
    benchmark::DoNotOptimize(changed);
  }
}

BENCHMARK_REGISTER_F(RandomData, TrackChangeOnce)
    ->RangeMultiplier(2)
    ->Range(8, 8 << 5);
BENCHMARK_REGISTER_F(RandomData, TrackChangeIncremental)
    ->RangeMultiplier(2)
    ->Range(8, 8 << 5);

BENCHMARK_MAIN();