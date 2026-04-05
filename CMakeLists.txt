cmake_minimum_required(VERSION 3.20)

project(NucleAliDP VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(alignment_core
    src/alignment.c
    src/instance_io.c
    src/DATA_struct.c
)

target_include_directories(alignment_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(MSVC)
    target_compile_options(alignment_core PRIVATE /W4)
else()
    target_compile_options(alignment_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(WIN32)
    target_link_libraries(alignment_core PRIVATE psapi)
endif()

add_executable(align_cli src/main.c)
target_link_libraries(align_cli PRIVATE alignment_core)

add_executable(test_small tests/small_tests.c)
target_link_libraries(test_small PRIVATE alignment_core)

add_executable(test_dp_all tests/testingDP_ALL_INST.c)
target_link_libraries(test_dp_all PRIVATE alignment_core)

add_executable(test_nm_all tests/testingNM_ALL_INST.c)
target_link_libraries(test_nm_all PRIVATE alignment_core)

add_executable(test_dist_compare tests/DIST1vsDIST2.c)
target_link_libraries(test_dist_compare PRIVATE alignment_core)

include(CTest)
if(BUILD_TESTING)
    add_test(
        NAME smoke_cli
        COMMAND align_cli --input ${CMAKE_CURRENT_SOURCE_DIR}/instances/Inst_0000010_7.adn --solver sol2
    )
endif()

