cmake_minimum_required(VERSION 4.3)
project(WienerSimulation)

set(CMAKE_CXX_STANDARD 20)

add_executable(WienerSimulation src/main.cpp
        src/exactGBM.hpp
        src/exactGBM.cpp
        src/util.hpp
        src/eulerMaruyama.cpp
        src/eulerMaruyama.hpp
        src/pricer.cpp
        src/pricer.hpp)
