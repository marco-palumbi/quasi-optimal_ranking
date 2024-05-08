cmake_minimum_required(VERSION 3.21)
project(rank_unrank C)

# help macro
macro(log var)
    message(STATUS "${var}: ${${var}}")
endmacro()

# Find the GMP library
find_library(GMP_LIBRARY gmp HINTS /usr/local/Cellar/gmp/6.3.0/lib/)

# Other not used flags: -fsanitize=address -Wformat -Wformat-security -Wall -Wextra

execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
        OUTPUT_VARIABLE GCC_VERSION)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -funroll-loops")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -funroll-loops -fsanitize=address -m64 -Wformat=0 -Werror -Wno-unused-function -Wno-unused-result -Wno-strict-prototypes -Wunused-value -Wunused-variable -Wundef")#-Wcast-align
else()
    message(FATAL_ERROR "CMAKE_BUILD_TYPE not valid")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I /usr/local/Cellar/gmp/6.3.0/include/")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
log(CMAKE_MODULE_PATH)
include(stm32f4discovery)

log(CMAKE_C_COMPILER)
log(CMAKE_C_FLAGS)

set(CMAKE_C_STANDARD 11)

if(PARAM_N1)
add_compile_definitions(PARAM_N1=${PARAM_N1})
endif()

if(NOT ("${toolchain}" STREQUAL "arm-none-eabi-libopencm3"))
    set(TEST_ITERATIONS 1000000)
    set(TEST_ITERATIONS_BNNF 100000)
else()
    set(TEST_ITERATIONS 1000)
    set(TEST_ITERATIONS_BNNF 100)
endif()

# Test lehmer bn
add_custom_exe(test_lehmer_bn test/test_lehmer_bn.c)
target_sources(test_lehmer_bn PRIVATE src/bn.c src/lehmer_bn.c src/permutations.c)
target_compile_definitions(test_lehmer_bn PRIVATE USE_FACTORIAL=1 N_ITERATIONS=${TEST_ITERATIONS})

add_custom_exe(test_lehmer_bn_no-fact test/test_lehmer_bn.c)
target_sources(test_lehmer_bn_no-fact PRIVATE src/bn.c src/lehmer_bn.c src/permutations.c)
target_compile_definitions(test_lehmer_bn_no-fact PRIVATE USE_FACTORIAL=0 N_ITERATIONS=${TEST_ITERATIONS_BNNF})

if(NOT ("${toolchain}" STREQUAL "arm-none-eabi-libopencm3"))
# Test lehmer gmp
add_custom_exe(test_lehmer_gmp test/test_lehmer_gmp.c)
target_sources(test_lehmer_gmp PRIVATE src/lehmer_gmp.c src/permutations.c)
target_link_libraries(test_lehmer_gmp PRIVATE ${GMP_LIBRARY})
target_compile_definitions(test_lehmer_gmp PRIVATE USE_FACTORIAL=1 N_ITERATIONS=${TEST_ITERATIONS})

add_custom_exe(test_lehmer_gmp_no-fact test/test_lehmer_gmp.c)
target_sources(test_lehmer_gmp_no-fact PRIVATE src/lehmer_gmp.c src/permutations.c)
target_link_libraries(test_lehmer_gmp_no-fact PRIVATE ${GMP_LIBRARY})
target_compile_definitions(test_lehmer_gmp_no-fact PRIVATE USE_FACTORIAL=0 N_ITERATIONS=${TEST_ITERATIONS})
endif()

# Test bonet bn
add_custom_exe(test_bonet_bn test/test_bonet_bn.c)
target_sources(test_bonet_bn PRIVATE src/bn.c src/bonet_bn.c src/permutations.c)
target_compile_definitions(test_bonet_bn PRIVATE USE_FACTORIAL=1 N_ITERATIONS=${TEST_ITERATIONS})

add_custom_exe(test_bonet_bn_no-fact test/test_bonet_bn.c)
target_sources(test_bonet_bn_no-fact PRIVATE src/bn.c src/bonet_bn.c src/permutations.c)
target_compile_definitions(test_bonet_bn_no-fact PRIVATE USE_FACTORIAL=0 N_ITERATIONS=${TEST_ITERATIONS_BNNF})

if(NOT ("${toolchain}" STREQUAL "arm-none-eabi-libopencm3"))
# Test bonet gmp
add_custom_exe(test_bonet_gmp test/test_bonet_gmp.c)
target_sources(test_bonet_gmp PRIVATE src/bonet_gmp.c src/permutations.c)
target_link_libraries(test_bonet_gmp PRIVATE ${GMP_LIBRARY})
target_compile_definitions(test_bonet_gmp PRIVATE USE_FACTORIAL=1 N_ITERATIONS=${TEST_ITERATIONS})

add_custom_exe(test_bonet_gmp_no-fact test/test_bonet_gmp.c)
target_sources(test_bonet_gmp_no-fact PRIVATE src/bonet_gmp.c src/permutations.c)
target_link_libraries(test_bonet_gmp_no-fact PRIVATE ${GMP_LIBRARY})
target_compile_definitions(test_bonet_gmp_no-fact PRIVATE USE_FACTORIAL=0 N_ITERATIONS=${TEST_ITERATIONS})
endif()

# Test quasi-optimal lehmer
add_custom_exe(test_lehmer_quasi-optimal test/test_lehmer_quasi-optimal.c)
target_sources(test_lehmer_quasi-optimal PRIVATE src/divide_tables.c src/lehmer_quasi-optimal.c src/permutations.c)
target_compile_definitions(test_lehmer_quasi-optimal PRIVATE N_ITERATIONS=${TEST_ITERATIONS})

if(NOT ("${toolchain}" STREQUAL "arm-none-eabi-libopencm3"))
# Test quasi-optimal lehmer avx2
add_custom_exe(test_lehmer_quasi-optimal_avx2 test/test_lehmer_quasi-optimal.c)
target_sources(test_lehmer_quasi-optimal_avx2 PRIVATE src/divide_tables.c src/lehmer_quasi-optimal_avx2.c src/permutations.c)
target_compile_definitions(test_lehmer_quasi-optimal_avx2 PRIVATE N_ITERATIONS=${TEST_ITERATIONS})
target_compile_options(test_lehmer_quasi-optimal_avx2 PRIVATE  -mavx2;-mpclmul;-msse4.2;-maes)
target_link_options(test_lehmer_quasi-optimal_avx2 PRIVATE -fuse-ld=lld )
target_link_options(test_lehmer_quasi-optimal_avx2 PRIVATE -Wl,--shuffle-sections -Wl,*=0)
target_compile_options(test_lehmer_quasi-optimal_avx2 PRIVATE "SHELL:-mllvm -align-all-functions=6" "SHELL:-mllvm -align-all-nofallthru-blocks=6")
endif()

# Test fast
add_custom_exe(test_fast test/test_fast.c)
target_sources(test_fast PRIVATE src/fast.c src/permutations.c)
target_compile_definitions(test_fast PRIVATE N_ITERATIONS=${TEST_ITERATIONS})
