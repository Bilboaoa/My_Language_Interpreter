add_executable(integration_tests
    test_parser.cpp
    ../../src/lexer.cpp
    ../../include/error.hpp
    ../../include/interpreter_exception.hpp
    ../../include/position.hpp
    ../../src/parser.cpp
    ../../include/asTree.hpp
    ../../src/asTree.cpp
)

target_include_directories(integration_tests PRIVATE
    ../../src
    ../../include
    ${Catch2_SOURCE_DIR}/src
)

target_link_libraries(integration_tests PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(integration_tests)
