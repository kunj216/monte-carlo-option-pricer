#!/bin/bash
set -e

cmake -S . -B cmake-build-debug
cmake --build cmake-build-debug
./cmake-build-debug/WienerSimulation > path.csv
python3 plotting.py