#include "src.hpp"
#include <iostream>

int main() {
  std::string regex, str;
  std::cin >> regex >> str;

  Grammar::RegexChecker checker(regex);
  std::cout << (checker.Check(str) ? "Yes" : "No") << std::endl;

  return 0;
}