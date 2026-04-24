#!/bin/bash

echo "Testing simple case a:"
echo "a a" | ./code
echo "Expected: Yes"

echo ""
echo "Testing simple case a doesn't match b:"
echo "a b" | ./code
echo "Expected: No"

echo ""
echo "Testing a+:"
echo "a+ aaa" | ./code
echo "Expected: Yes"

echo ""
echo "Testing a+ with empty:"
echo "a+ " | ./code
echo "Expected: No"

echo ""
echo "Testing a*:"
echo "a* aaa" | ./code
echo "Expected: Yes"

echo ""
echo "Testing a* with empty:"
echo "a* " | ./code
echo "Expected: Yes"

echo ""
echo "Testing a?:"
echo "a? a" | ./code
echo "Expected: Yes"

echo ""
echo "Testing a? with empty:"
echo "a? " | ./code
echo "Expected: Yes"

echo ""
echo "Testing a|b:"
echo "a|b a" | ./code
echo "Expected: Yes"

echo ""
echo "Testing a|b with c:"
echo "a|b c" | ./code
echo "Expected: No"