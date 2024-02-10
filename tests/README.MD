#### Configure
go to root path cd .. cmake -S . -B build  
then execute cmake --build . -j 12  
 GTEST_COLOR=1 ctest --test-dir . --output-on-failure -j12