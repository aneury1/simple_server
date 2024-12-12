#!/usr/bin/env bash

# Ensure the script is passed a clang-tidy config file or arguments
if [ -z "$1" ]; then
    echo "Usage: $0 <clang-tidy-config-or-checks>"
    exit 1
fi
#  "-checks=modernize-*"
# Find all .cpp files and run clang-tidy
find . -type f -name "*.cpp" | xargs -I{} clang-tidy "$1" {} -- -std=c++17