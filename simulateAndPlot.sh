#!/bin/bash
set -e

cmake -S . -B cmake-build-debug
cmake --build cmake-build-debug
./cmake-build-debug/WienerSimulation > /tmp/path.csv
python3 plotting.py