#include "rand_int_benchmark.h"

#include "bwt.hpp"

#include <climits>
#include <cstdint>
#include <optional>
#include <random>
#include <vector>

bool rand_int_benchmark(int n){
  //Create a random array of size n.
  std::vector<uint32_t> values = create_pseudo_random_ints(n);

  // bwt transform
  // TODO: time
  const bwt::Encoded<uint32_t> encoded = bwt::encode(values);

  // ibwt
  //TODO: time
  const std::optional<std::vector<uint32_t>> decoded_values =
      bwt::decode(encoded);
  //TODO(kevintownsend): assert equal
  if(!decoded_values)
    return false;
  return values == *decoded_values;
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
