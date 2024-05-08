#################################################
# lib opencm3

set(OPENCM3DIR   libopencm3)
set(OPENCM3LIB   opencm3_stm32f4)
set(OPENCM3BUILD stm32/f4)

set(LIBOPENCM3_COMMIT b1d8a4c5)


add_custom_target(buildlibopencm3  DEPENDS ${PROJECT_SOURCE_DIR}/${OPENCM3DIR}/Makefile ${PROJECT_SOURCE_DIR}/${OPENCM3DIR}/lib/lib${OPENCM3LIB}.a)
# clone libopencm3
add_custom_command(
    OUTPUT  ${PROJECT_SOURCE_DIR}/${OPENCM3DIR}/Makefile
    COMMAND rm -rf ${OPENCM3DIR}
    COMMENT "### runn git submodule update --init --recursive"
    COMMAND git submodule update --init --recursive
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
# build libopencm3
add_custom_command(
    OUTPUT ${PROJECT_SOURCE_DIR}/${OPENCM3DIR}/lib/lib${OPENCM3LIB}.a
    COMMAND make -C ${OPENCM3DIR} TARGETS=${OPENCM3BUILD}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_library(libopencm3 STATIC IMPORTED GLOBAL)
add_dependencies(libopencm3 buildlibopencm3)
set_target_properties(libopencm3 PROPERTIES
            IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/${OPENCM3DIR}/lib/lib${OPENCM3LIB}.a
#            INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/${OPENCM3DIR}/include
            )
set(LIBOPENCM3_INCLUDE ${PROJECT_SOURCE_DIR}/${OPENCM3DIR}/include)
######################################################


set(USE_QEMU False CACHE STRING "Use qemu emulator for M4 target. It doesn't affect other tatgets")

if("${toolchain}" STREQUAL "arm-none-eabi-libopencm3")
    if(USE_QEMU)
      add_compile_options(-DUSE_QEMU)
      set(RUNTEST bash  ${PROJECT_SOURCE_DIR}/tests/run_test.sh -v -h 1 -b )
    else()
      set(RUNTEST bash  ${PROJECT_SOURCE_DIR}/tests/run_test.sh -v -b )
    endif()
    add_compile_options(-DSTM32F4)
    add_link_options(-L${PROJECT_SOURCE_DIR}/libopencm3/lib)
    add_link_options(-T${PROJECT_SOURCE_DIR}/hw/stm32f405x6.ld)
    include_directories(${LIBOPENCM3_INCLUDE})
    link_libraries(libopencm3)
    add_library(hal_lib OBJECT ${PROJECT_SOURCE_DIR}/hw/hal-stm32f4.c ${PROJECT_SOURCE_DIR}/hw/syscall_wrappers.c)
    # disable cppcheck and clang-tidy for hal-stm32f4.c
    set_target_properties(hal_lib PROPERTIES C_CPPCHECK "" C_CLANG_TIDY "")
    link_libraries(hal_lib)
    set(EXT .bin)
endif()

#add_executeble with additional objcopy step when needed
macro(add_custom_exe)
    add_executable(${ARGV})
    target_compile_definitions(${ARGV0} PRIVATE TARGET_NAME="${ARGV0}")
    if("${EXT}" STREQUAL ".bin" AND (NOT KEEP_ELF)) ## add objcopy command
    add_custom_command(
        TARGET ${ARGV0} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary ${ARGV0} ${ARGV0}
        DEPENDS ${ARGV0})
    endif()
endmacro()

include_directories(${PROJECT_SOURCE_DIR}/hw)

add_library(test_randomlib STATIC ${PROJECT_SOURCE_DIR}/hw/hw_randombytes.c)
link_libraries(test_randomlib)

add_compile_definitions(USERDTSC)
