#include "util.h"

#include <bits/stdc++.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <stdexcept>

namespace toy {
namespace util {

std::string ExeGeneratedCode() {
  std::string cmd = "../build/moon ../out/outcode.m";
  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"),
                                                pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  std::cout << result;
  return result;
};

std::string TrimString(const std::string& str) {
  auto wsfront = std::find_if_not(str.begin(), str.end(),
                                  [](int c) { return std::isspace(c); });
  auto wsback = std::find_if_not(str.rbegin(), str.rend(), [](int c) {
                  return std::isspace(c);
                }).base();
  return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
};

std::vector<std::string> SplitStrByNewline(const std::string& str) {
  auto result = std::vector<std::string>{};
  std::stringstream ss(str);
  for (std::string line; std::getline(ss, line, '\n');) result.push_back(line);
  return result;
};

}  // namespace util
}  // namespace toy
