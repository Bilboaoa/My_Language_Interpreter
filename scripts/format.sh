#!/bin/bash
echo "Formatting all source files..."
find ../ \( -name '*.cpp' -o -name '*.h' \) -exec clang-format -i {} \;
echo "Done."
