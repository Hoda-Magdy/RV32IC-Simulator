cmake_minimum_required(VERSION 3.24)
project(RV32IC_Simulator)

set(CMAKE_CXX_STANDARD 14)

add_executable(RV32IC_Simulator
        .idea/.gitignore
        .idea/modules.xml
        .idea/RV32IC-Simulator.iml
        .idea/vcs.xml
        .idea/workspace.xml
        .gitignore
        main.cpp
        README.md generate_test_file.cpp)
