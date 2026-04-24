#include "src.hpp"
#include <iostream>
#include <cassert>

int main() {
  // Test concatenation
  {
    Grammar::RegexChecker checker("ab");
    assert(checker.Check("ab") == true);
    assert(checker.Check("a") == false);
    assert(checker.Check("b") == false);
    assert(checker.Check("aba") == false);
  }

  // Test union
  {
    Grammar::RegexChecker checker("a|b");
    assert(checker.Check("a") == true);
    assert(checker.Check("b") == true);
    assert(checker.Check("ab") == false);
  }

  // Test combination
  {
    Grammar::RegexChecker checker("ab+a|ba*");
    assert(checker.Check("aba") == true);
    assert(checker.Check("abb") == true);
    assert(checker.Check("ba") == true);
    assert(checker.Check("baa") == true);
    assert(checker.Check("baaa") == true);
    assert(checker.Check("b") == true);
  }

  std::cout << "All tests passed!" << std::endl;
  return 0;
}