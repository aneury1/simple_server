#!/usr/bin/bash
gcovr -r . --html --html-details -o ./test/code-coverage/coverage_report.html
gcovr -r .
lcov --capture --directory . --output-file ./test/code-coverage/coverage.info
lcov --remove coverage.info '/usr/*' '*/test/*' --output-file ./test/code-coverage/coverage_filtered.info
genhtml ./test/code-coverage/coverage_filtered.info --output-directory ./test/code-coverage/coverage_report
# view 
# xdg-open coverage_report/index.html
