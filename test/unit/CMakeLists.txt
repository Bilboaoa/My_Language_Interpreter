add_executable(unit_tests
    test_lexer.cpp
    ../../src/lexer.cpp
    ../../include/error.hpp
    ../../include/interpreter_exception.hpp
    ../../include/position.hpp
)

target_include_directories(unit_tests PRIVATE
    ../../src
    ../../include
    ${Catch2_SOURCE_DIR}/src
)

target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(unit_tests)
