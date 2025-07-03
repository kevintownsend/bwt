#include "rand_int_benchmark.h"

#include "bwt.hpp"

#include <vector>
#include <cstdint>
#include <random>
#include <climits>

bool rand_int_benchmark(int n){
  //Create a random array of size n.
  std::vector<uint32_t> values = create_pseudo_random_ints(n);
  //TODO(kevintownsend): bwt transform
  auto key = townsend::algorithm::bwtEncode(values.begin(), values.end());
  //TODO(kevintownsend): ibwt
  //townsend::algorithm::bwtDecode(values.begin(), values.end(), key);
  //TODO(kevintownsend): assert equal
  return true;
}

std::vector<uint32_t> create_pseudo_random_ints(int n) {
  std::vector<uint32_t> values(n);
  std::default_random_engine generator;
  std::uniform_int_distribution<uint32_t> distribution(0, UINT32_MAX);
  for(int i = 0; i < n; i++) {
    values[i] = distribution(generator);
  }
  return values;
}
