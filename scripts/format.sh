#!/bin/bash
echo "Formatting all source files..."
find ../ \( -name '*.cpp' -o -name '*.hpp' \) -exec clang-format -i {} \;
echo "Done."
