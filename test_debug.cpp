#include "src.hpp"
#include <iostream>

int main() {
  Grammar::RegexChecker checker("ab+a");
  std::cout << "Testing 'abb': " << checker.Check("abb") << std::endl;
  std::cout << "Testing 'aba': " << checker.Check("aba") << std::endl;
  return 0;
}