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

format:
    clang-format -i $(find src include test \( -name '*.cpp' -o -name '*.hpp' \) -not -path "./build/*" -not -path "./_deps/*" )

clean:
    rm -rf build
