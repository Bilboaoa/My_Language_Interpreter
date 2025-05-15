cmake_minimum_required(VERSION 3.14)
project(MyInterpreter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_COVERAGE "Enable code coverage" OFF)
if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(STATUS "Enabling code coverage")
    add_compile_options(-fprofile-arcs -ftest-coverage)
    add_link_options(--coverage)
endif()

file(GLOB_RECURSE SRC_FILES 
    "src/*.cpp"
    "src/visitators/*.cpp"
)

add_executable(bibl ${SRC_FILES})

target_include_directories(bibl 
    PRIVATE 
    include
    include/visitors
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(-Wall -Wextra -pedantic -Werror)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g)
    endif()
endif()

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(Catch2)

enable_testing()
add_subdirectory(test)


if(ENABLE_COVERAGE)
    find_program(LCOV_PATH lcov REQUIRED)
    find_program(GENHTML_PATH genhtml REQUIRED)
    
    add_custom_target(coverage
        COMMAND ${LCOV_PATH} --capture --directory . --output-file coverage.info --ignore-errors source
        COMMAND ${LCOV_PATH} --remove coverage.info '*/test/*' '*/_deps/*' '*/usr/include/*' --output-file coverage-filtered.info
        COMMAND ${GENHTML_PATH} coverage-filtered.info --output-directory coverage_report
        COMMAND ${CMAKE_COMMAND} -E echo "Coverage report: file://${CMAKE_BINARY_DIR}/coverage_report/index.html"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS bibl test
    )
endif()