#!/bin/bash
echo "Formatting all source files..."

find ../src ../include ../test \
    \( -name '*.cpp' -o -name '*.hpp' \) \
    -not -path "./build/*" \
    -not -path "./_deps/*" \
    -exec clang-format -i {} \;

echo "Done."
