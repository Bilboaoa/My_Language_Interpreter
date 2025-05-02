build:
    mkdir -p build
    cd build && cmake ..
    cmake --build build

debug:
    mkdir -p build
    cd build && cmake ..
    cmake --build build --config Debug

release:
    mkdir -p build
    cd build && cmake ..
    cmake --build build --config Release

test: build
    cmake --build build --target test

test-cov:
    rm -rf build
    mkdir -p build && cd build && cmake -DENABLE_COVERAGE=ON ..
    
    cmake --build build
    cd build && ctest --output-on-failure
    
    cd build && \
    lcov --capture --directory . --output-file coverage.info --ignore-errors source && \
    lcov --remove coverage.info \
        '*/test/*' \
        '*/_deps/*' \
        '*/usr/include/*' \
        --output-file coverage-filtered.info && \
    genhtml coverage-filtered.info --output-directory coverage_report && \
    echo "Coverage report available at build/coverage_report/index.html"

format:
    clang-format -i $(find src include test \( -name '*.cpp' -o -name '*.hpp' \) -not -path "./build/*" -not -path "./_deps/*" )

clean:
    rm -rf build

doc:
    doxygen Doxyfile

lint:
    cppcheck --enable=all --suppress=missingIncludeSystem include/* src/*