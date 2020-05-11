cmake_minimum_required(VERSION 3.0.0)
project(MyProject VERSION 0.1.0)

file(GLOB_RECURSE SOURCES "firmware/src/*.c")
file(GLOB_RECURSE SOURCES_CXX "firmware/src/*.cpp")
list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/config/default/startup_xc32.c)
list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/app.c)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${SOURCES_CXX})

include_directories(
    firmware/src
    firmware/src/config/default
    firmware/src/packs/CMSIS/CMSIS/Core/Include
    firmware/src/packs/ATSAMD51P19A_DFP
    firmware/src/third_party/rtos/FreeRTOS/Source/include
    firmware/src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F
)

set_target_properties(${PROJECT_NAME}.elf PROPERTIES LINK_DEPENDS ${CMAKE_SOURCE_DIR}/firmware/linker.ld)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c11 -g -Os")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c++14 -g -Os")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-gc-sections \"-T${CMAKE_SOURCE_DIR}/firmware/linker.ld\" -g -Os")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

add_custom_target(${PROJECT_NAME}.bin ALL DEPENDS ${PROJECT_NAME}.elf)
add_custom_command(TARGET ${PROJECT_NAME}.bin COMMAND arm-none-eabi-objcopy ARGS -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin)
