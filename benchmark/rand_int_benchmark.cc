#include "rand_int_benchmark.h"

#include "bwt.hpp"

#include <vector>
#include <cstdint>
#include <random>
#include <climits>

bool rand_int_benchmark(int n){
  //Create a random array of size n.
  std::vector<uint32_t> values = create_pseudo_random_ints(n);

  // bwt transform
  // TODO: time
  std::vector<uint32_t> encoded_values;
  size_t key;
  std::tie(encoded_values, key) = bwt::bwt(values.begin(), values.end());

  // ibwt
  std::vector<uint32_t> decoded_values;
  bool success;

  //TODO: time
  std::tie(decoded_values, success) = bwt::iBwt(encoded_values.begin(), encoded_values.end(), key);
  //TODO(kevintownsend): assert equal
  if(!success)
    return false;
  for(size_t i = 0; i < values.size(); i++) {
    if(values[i] != decoded_values[i])
      return false;
  }
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
