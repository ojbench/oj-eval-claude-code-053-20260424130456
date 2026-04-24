#include "src.hpp"
#include <iostream>
#include <cassert>

int main() {
  // Test more complex patterns
  {
    // Test concatenation with +
    Grammar::RegexChecker checker("a+b+");
    assert(checker.Check("ab") == true);
    assert(checker.Check("aab") == true);
    assert(checker.Check("abb") == true);
    assert(checker.Check("aabb") == true);
    assert(checker.Check("a") == false);
    assert(checker.Check("b") == false);
  }

  // Test ? with union
  {
    Grammar::RegexChecker checker("a?b?");
    assert(checker.Check("a") == true);
    assert(checker.Check("b") == true);
    assert(checker.Check("ab") == true);
    assert(checker.Check("aa") == false);
    assert(checker.Check("bb") == false);
  }

  // Test a* pattern
  {
    Grammar::RegexChecker checker("a*");
    assert(checker.Check("") == true);
    assert(checker.Check("a") == true);
    assert(checker.Check("aa") == true);
    assert(checker.Check("aaa") == true);
    assert(checker.Check("b") == false);
  }

  // Test complex union
  {
    Grammar::RegexChecker checker("a|bb|ab");
    assert(checker.Check("a") == true);
    assert(checker.Check("b") == false);
    assert(checker.Check("bb") == true);
    assert(checker.Check("ab") == true);
    assert(checker.Check("ba") == false);
  }

  std::cout << "All edge case tests passed!" << std::endl;
  return 0;
}