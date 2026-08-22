#!/bin/bash
set -e

cmake -S . -B cmake-build-debug
cmake --build cmake-build-debug
./cmake-build-debug/WienerSimulation path --paths 10
python3 ./scripts/plotting.py