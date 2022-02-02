cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS -pthread)

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
    A64Core/MOS6526A.cpp
    A64Core/MOS6569.cpp
    A64Core/Ram.cpp
    A64Core/Util.cpp 
    main.cpp)

#A64Core/UTestOpcode.cpp

add_executable(
    ${PROJECT_NAME}
    ${SRC}
)

message(STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")

target_compile_options(
    ${PROJECT_NAME}
    PUBLIC "-pthread"
)

target_link_libraries( ${PROJECT_NAME} )


