#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <vector>

#include "benchmark/rand_int_benchmark.h"

namespace {

void print_header() {
  std::cout << std::left << std::setw(12) << "Workload" << std::right
            << std::setw(10) << "Elements" << std::setw(11) << "Bytes"
            << std::setw(11) << "Enc (ms)" << std::setw(11) << "Enc MB/s"
            << std::setw(11) << "Dec (ms)" << std::setw(11) << "Dec MB/s"
            << std::setw(18) << "AvgRun (In->Out)" << std::setw(8) << "Status"
            << '\n';
  std::cout << std::string(101, '-') << '\n';
}

void print_row(const BenchmarkResult& r) {
  std::cout << std::left << std::setw(12) << r.name << std::right
            << std::setw(10) << r.elements << std::setw(11) << r.bytes
            << std::fixed << std::setprecision(2) << std::setw(11)
            << r.encode_ms << std::setw(11) << r.encode_mb_per_s
            << std::setw(11) << r.decode_ms << std::setw(11)
            << r.decode_mb_per_s << std::setw(8) << r.avg_run_before << "->"
            << std::left << std::setw(8) << r.avg_run_after << std::right
            << std::setw(8) << (r.verified ? "OK" : "FAIL") << '\n';
}

}  // namespace

int main(int argc, char* argv[]) {
  bool quick = false;
  for (int i = 1; i < argc; ++i) {
    const std::string_view arg(argv[i]);
    if (arg == "--quick") {
      quick = true;
    } else if (arg == "-h" || arg == "--help") {
      std::cout << "Usage: " << argv[0] << " [--quick]\n";
      return 0;
    } else {
      std::cerr << "Unknown option: " << arg << '\n';
      return 1;
    }
  }

  const std::vector<std::size_t> sizes =
      quick ? std::vector<std::size_t>{1024, 16384, 65536}
            : std::vector<std::size_t>{1024, 16384, 131072, 1048576};

  print_header();
  bool all_ok = true;
  for (std::size_t n : sizes) {
    const std::vector<BenchmarkResult> results = {
        run_char_benchmark("text", create_repetitive_text(n)),
        run_char_benchmark("dna", create_repetitive_dna(n)),
        run_uint8_benchmark("low_bin", create_low_entropy_binary(n)),
        run_uint32_benchmark("rand_u32", create_pseudo_random_ints(n)),
    };
    for (const BenchmarkResult& r : results) {
      print_row(r);
      all_ok = all_ok && r.verified;
    }
  }
  return all_ok ? 0 : 1;
}
