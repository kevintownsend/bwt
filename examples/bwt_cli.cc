/**
 * Copyright 2025 Kevin Townsend
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "bwt.hpp"

namespace {

void print_usage(std::ostream& out, const char* prog) {
  out << "Usage: " << prog << " [<text> | -f <path>]\n"
      << "Reads stdin when no arguments are given.\n";
}

bool read_all_bytes(std::istream& in, std::vector<char>& out) {
  out.assign(std::istreambuf_iterator<char>(in),
             std::istreambuf_iterator<char>());
  return !in.bad();
}

void print_encoded_preview(const bwt::Encoded<char>& encoded) {
  if (encoded.data.size() > 1000) {
    return;
  }
  for (std::size_t i = 0; i < encoded.data.size(); ++i) {
    if (i == encoded.key) {
      std::cout.put('$');
    }
    std::cout.put(encoded.data[i]);
  }
  if (encoded.key == encoded.data.size()) {
    std::cout.put('$');
  }
  std::cout.put('\n');
}

}  // namespace

int main(int argc, char* argv[]) {
  std::vector<char> input;

  if (argc == 1) {
    if (!read_all_bytes(std::cin, input)) {
      std::cerr << "Error: failed reading standard input\n";
      return 1;
    }
  } else if (argc == 2) {
    const std::string_view arg(argv[1]);
    if (arg == "-h" || arg == "--help") {
      print_usage(std::cout, argv[0]);
      return 0;
    }
    if (arg == "-f") {
      std::cerr << "Error: -f requires a file path\n";
      print_usage(std::cerr, argv[0]);
      return 1;
    }
    input.assign(arg.begin(), arg.end());
  } else if (argc == 3 && std::string_view(argv[1]) == "-f") {
    std::ifstream file(argv[2], std::ios::binary);
    if (!file) {
      std::cerr << "Error: unable to open file '" << argv[2] << "'\n";
      return 1;
    }
    if (!read_all_bytes(file, input)) {
      std::cerr << "Error: failed reading file '" << argv[2] << "'\n";
      return 1;
    }
  } else {
    print_usage(std::cerr, argv[0]);
    return 1;
  }

  const bwt::Encoded<char> encoded = bwt::encode(input);
  print_encoded_preview(encoded);

  const std::optional<std::vector<char>> decoded = bwt::decode(encoded);
  if (!decoded || *decoded != input) {
    std::cerr << "Error: round-trip decode verification failed\n";
    return 1;
  }

  if (decoded->size() <= 1000) {
    std::cout.write(decoded->data(),
                    static_cast<std::streamsize>(decoded->size()));
    std::cout.put('\n');
  }
  return 0;
}
