cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS -pthread)

add_definitions(-DEMBEDDED_ROMS)

project(cmos6510)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED_ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(SRC
    A64Core/BasicRom.cpp
    A64Core/Bus.cpp
    A64Core/CBM64Main.cpp
    A64Core/CharRom.cpp
    A64Core/Device.cpp
    A64Core/KernalRom.cpp
    A64Core/MOS6510.cpp
    A64Core/MOS6510Debug.cpp
    A64Core/MOS6526_CIA1.cpp
    A64Core/MOS6526_CIA2.cpp
    A64Core/MOS6569.cpp
    A64Core/Ram.cpp
    A64Core/Util.cpp 
    A64Core/Common.cpp 
    main.cpp)

add_custom_command(OUTPUT roms.o
    COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/roms && ld -r -b binary -o ${CMAKE_CURRENT_BINARY_DIR}/roms.o KERNAL.ROM BASIC.ROM CHAR.ROM
    COMMAND objcopy --rename-section .data=.rodata,alloc,load,readonly,data,contents ${CMAKE_CURRENT_BINARY_DIR}/roms.o ${CMAKE_CURRENT_BINARY_DIR}/roms.o)

add_executable(
    ${PROJECT_NAME}
    ${SRC}
    roms.o
)

message(STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")

target_compile_options(
    ${PROJECT_NAME}
    PUBLIC "-pthread"
)

target_link_libraries( ${PROJECT_NAME} )


