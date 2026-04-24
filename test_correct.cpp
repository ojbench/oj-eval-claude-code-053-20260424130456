#include "src.hpp"
#include <iostream>
#include <cassert>

int main() {
  // Test a+b - should match one or more 'a' followed by 'b'
  {
    Grammar::RegexChecker checker("a+b");
    assert(checker.Check("ab") == true);   // One a, then b
    assert(checker.Check("aab") == true);  // Two a's, then b
    assert(checker.Check("aaab") == true); // Three a's, then b
    assert(checker.Check("b") == false);   // No a
    assert(checker.Check("a") == false);   // No b
    assert(checker.Check("abb") == false); // Extra b
  }

  // Test ba* - should match 'b' followed by zero or more 'a'
  {
    Grammar::RegexChecker checker("ba*");
    assert(checker.Check("b") == true);     // Zero a's
    assert(checker.Check("ba") == true);    // One a
    assert(checker.Check("baa") == true);   // Two a's
    assert(checker.Check("baaa") == true);  // Three a's
    assert(checker.Check("bb") == false);   // b followed by b, not a
    assert(checker.Check("bab") == false);  // b followed by a then b
  }

  // Test a+b|ba* union - should match either pattern
  {
    Grammar::RegexChecker checker("a+b|ba*");
    assert(checker.Check("ab") == true);    // Matches a+b
    assert(checker.Check("aab") == true);   // Matches a+b
    assert(checker.Check("b") == true);     // Matches ba*
    assert(checker.Check("ba") == true);    // Matches ba*
    assert(checker.Check("baa") == true);   // Matches ba*
    assert(checker.Check("abb") == false);  // Doesn't match either
    assert(checker.Check("bb") == false);   // Doesn't match either
    assert(checker.Check("a") == false);    // Doesn't match either
  }

  std::cout << "All tests passed!" << std::endl;
  return 0;
}