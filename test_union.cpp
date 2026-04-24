#include "src.hpp"
#include <iostream>

int main() {
  Grammar::RegexChecker checker("a+b|ba*");

  std::cout << "a+b with 'ab': " << checker.Check("ab") << std::endl;
  std::cout << "a+b with 'abb': " << checker.Check("abb") << std::endl;
  std::cout << "ba* with 'b': " << checker.Check("b") << std::endl;
  std::cout << "ba* with 'ba': " << checker.Check("ba") << std::endl;
  std::cout << "ba* with 'baa': " << checker.Check("baa") << std::endl;
  std::cout << "Should all be 1 (true)" << std::endl;

  return 0;
}