set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE TEST_SOURCES "test_parser.cpp")
file(GLOB_RECURSE SRC_SOURCES
    "../../src/lexer.cpp"
    "../../src/parser.cpp"
    "../../src/asTree.cpp"
    "../../src/visitors/parserVisitor.cpp"
)

file(GLOB_RECURSE INCLUDE_HEADERS
    "../../include/error.hpp"
    "../../include/interpreter_exception.hpp"
    "../../include/position.hpp"
    "../../include/asTree.hpp"
    "../../include/visitors/astVisitor.hpp"
    "../../include/visitors/parserVisitor.hpp"
)

add_executable(integration_tests
    ${TEST_SOURCES}
    ${SRC_SOURCES}
)

target_include_directories(integration_tests PRIVATE
    ../../include
    ../../include/visitors
    ${Catch2_SOURCE_DIR}/src
)

target_link_libraries(integration_tests PRIVATE Catch2::Catch2WithMain)


include(CTest)
include(Catch)
catch_discover_tests(integration_tests)
