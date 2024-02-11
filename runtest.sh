#!/usr/bin/bash
cd ./build
 cmake --build . -j 12
 GTEST_COLOR=1 ctest --test-dir . --output-on-failure -j12